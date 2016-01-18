#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "ml_algo.h"
#include "svm_core.h"
#include "string.h"

static inline double sqrDistance(double* a1, double* b1, int size)
{
	double distance = 0;
	for (int i = 0; i < size; i++)
		distance += (a1[i] - b1[i]) * (a1[i] - b1[i]);
	return distance;
}


static bool labelRandomization(int* plabel, int* nlabel, int pfirst, int nfirst, int psize, int nsize)
{
	if (pfirst >= psize) return false;
	if (nfirst >= nsize) return false;
	/*if (plabel[pfirst] == 0) {
		for (int i = pfirst + 1; i < psize; i++)
			plabel[i] = -1;
		plabel[pfirst] = 1;
	} else {
	*/
	//for (int i = psize - 1; i > pfirst; i++)
	return true;

}


class Cluster
{
	private:
		double (*pdata)[VARS];
		double (*ndata)[VARS];
		//double* label;
		int* pgid; //group label
		int* ngid; //group label

		svm_parameter param;
		svm_problem problem;
		svm_model* model;

		int psize;
		int nsize;
		int size;
		int pncluster;
		int nncluster;
		int max_size;
		int max_ncluster; // max k value

	public:
		Cluster(double (*pdata)[VARS], double (*ndata)[VARS], int psize, int nsize){
			this->pdata = pdata;
			this->ndata = ndata;
			pgid = new int[psize];
			ngid = new int[nsize];
			this->psize = psize;
			this->nsize = nsize;
			size = psize + nsize;
			problem.l = 0;
			//problem.x = (svm_node**)(data);
			//problem.y = label;
			prepare_linear_parameters(param);
			model = NULL;
		}

		~Cluster() {
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (pgid != NULL) delete []pgid;
			if (ngid != NULL) delete []ngid;
		}


		bool doClustering() {
			pncluster = 0;
			nncluster = 0;
			int k;
			for(k = 1; k <= max_ncluster; k++) {
				if (kClustering(k) == true) {
					pncluster = k;
					nncluster = k;
					break;
				}
			}
			if (k > max_ncluster)
				return false;

			bool merged = true;
			while (merged) {
				merged = false;
				for (int i = 0; i < pncluster; i++) {
					for (int j = i; j < pncluster; j++) {
						if (pairSetLinearSeparable(true, i, j) == true) {
							mergeByGroupID(true, i, j);
							pncluster--;
							merged = true;
							j--;
						}
					}
				}
				for (int i = 0; i < nncluster; i++) {
					for (int j = i; j < nncluster; j++) {
						if (pairSetLinearSeparable(false, i, j) == true) {
							mergeByGroupID(false, i, j);
							nncluster--;
							merged = true;
							j--;
						}
					}
				}
			}

			return true;
		}


		void mergeByGroupID(bool positive, int i1, int i2) {
			int* gid = pgid;
			int cursize = psize;
			int curcluster = pncluster;
			if (!positive) {
				gid = ngid;
				cursize = nsize;
				curcluster = nncluster;
			}
			for (int m = 0; m < cursize; m++) {
				if (gid[m] == i2)
					gid[m] = i1;
				else if (gid[m] == curcluster)
					gid[m] = i2;
			}
		}	


		friend std::ostream& operator << (std::ostream& out, const SVM& svm) {
			return svm._print(out);
		}

		virtual std::ostream& _print(std::ostream& out) const {
			return out;
		}

		virtual int getProblemSize() {
			return problem.l;
		}

		virtual int predict(double* v) {
			if (v == NULL) return -2;
			return -1;
		}

	private:
		bool kClustering(int k) {
			kmeans(k, true);
			kmeans(k, false);
			for (int i = 0; i < k; i++) {
				for (int j = 0; j < k; j++) {
					if (pairLinearSeparable(i, j) == false)
						return false;
				}
			}
			return true;
		}

		int kmeans(int k, bool positive)
		{
			double (*data)[VARS];
			int* gid;
			if (positive) {
				data = pdata;
				gid = pgid;
			} else {
				data = ndata;
				gid = ngid;
			}
			if (k > max_size) return -1;
			if (k > size) return -1;
			if (k == size) {
				for (int i = 0; i < k; i++)
					gid[i] = i;
				return 0;
			}

			double** center = new double*[k];
			for (int i = 0; i < k; i++)
				center[i] = new double[VARS];
			double* distance = new double[k];
			int* g_members = new int[k];

			// randomly choose points as the initial means to begin K-means algorithm
			int duration = size / k;
			for (int last = 0, i = 0; i < k; i++) {
				last += rand() % duration + 1;
				for (int j = 0; j < VARS; j++)
					center[i][j] = data[last][j];
			}

			while (true) {	
				// regroup according to square distance.
				bool regrouped = false;
				for (int i = 0; i < size; i++) {
					int min_center = 0;
					double min_dis = DBL_MAX;
					for (int j = 0; j < k; j++) {
						distance[j] = sqrDistance(center[j], data[i], VARS);
						if (distance[j] < min_dis) {
							min_center = j;
							min_dis = distance[j];
						}
					}

					if (gid[i] != min_center) {
						regrouped = true;
						gid[i] = min_center;
					}
				}
				if (regrouped == false)
					break;

				// recalculate means for each group
				for (int i = 0; i < k; i++)
					g_members[i] = 0;
				for (int i = 0; i < size; i++) {
					g_members[gid[i]]++;
					for (int j = 0; j < VARS; j++)
						center[gid[i]][j] += data[i][j];
				}
				for (int i = 0; i < k; i++)
					for (int j = 0; j < VARS; j++)
						center[i][j] /= g_members[i];
			}


			for (int i = 0; i < k; i++)
				delete []center[i];
			delete []g_members;
			delete []distance;
			delete []center;
			return 0;
		}

		bool pairLinearSeparable(int p_gid, int n_gid)
		{
			double** train_data = new double*[size];
			double* train_label = new double[size];
			int idx = 0;
			for(int i = 0; i < psize; i++) {
				if (pgid[i] == p_gid) {
					train_data[idx] = pdata[i];
					train_label[idx++] = 1;
				}
			}
			for(int i = 0; i < nsize; i++) {
				if (ngid[i] == n_gid) {
					train_data[idx] = ndata[i];
					train_label[idx++] = -1;
				}
			}
			problem.l = idx;
			problem.x = (svm_node**)train_data;
			problem.y = train_label;

			const char* error_msg = svm_check_parameter(&problem, &param);
			if (error_msg) { 
				std::cout << "ERROR: " << error_msg << std::endl; 
				return -1; 
			}

			model = svm_train(&problem, &param);
			Equation* equation = new Equation();
			svm_model_visualization(model, *equation);
			svm_free_and_destroy_model(&model);

			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (Equation::calc(*equation, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
			}

			delete []train_data;
			delete []train_label;
			if (pass < problem.l)
				return false;
			return true;
		}

		bool pairSetLinearSeparable(bool positive, int i1, int i2)
		{
			double** train_data = new double*[size];
			double* train_label = new double[size];
			int idx = 0;
			double (*pair_data)[VARS] = pdata;
			int *pair_gid = pgid;
			int pair_label = 1;
			int pair_size = psize;
			double (*set_data)[VARS] = ndata;
			int set_label = -1;
			int set_size = nsize;
			if (!positive) {
				pair_data = ndata;
				pair_gid = ngid;
				pair_label = -1;
				pair_size = nsize;
				set_data = pdata;
				set_label = 1;
				set_size = psize;
			}

			for(int i = 0; i < pair_size; i++) {
				if ((pair_gid[i] == i1) || (pair_gid[i] == i2)) {
					train_data[idx] = pair_data[i];
					train_label[idx++] = pair_label;
				}
			}
			for(int i = 0; i < set_size; i++) {
				train_data[idx] = ndata[i];
				train_label[idx++] = set_label;
			}
			problem.l = idx;
			problem.x = (svm_node**)train_data;
			problem.y = train_label;

			const char* error_msg = svm_check_parameter(&problem, &param);
			if (error_msg) { 
				std::cout << "ERROR: " << error_msg << std::endl; 
				return -1; 
			}

			model = svm_train(&problem, &param);
			Equation* equation = new Equation();
			svm_model_visualization(model, *equation);
			svm_free_and_destroy_model(&model);

			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (Equation::calc(*equation, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
			}

			delete []train_data;
			delete []train_label;
			if (pass < problem.l)
				return false;
			return true;
		}
};

#endif /* _SVM_H */
