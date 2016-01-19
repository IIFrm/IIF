#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "ml_algo.h"
#include "svm_core.h"
#include "string.h"
#include <cmath>

static inline double sqrDistance(double* a1, double* b1, int size)
{
	double distance = 0;
	for (int i = 0; i < size; i++)
		distance += (a1[i] - b1[i]) * (a1[i] - b1[i]);
	return distance;
}


static bool groupLabelRandomization(int* pglabel, int* nglabel, int pfirst, int nfirst, int psize, int nsize, const unsigned int place)
{
	if (pfirst >= psize) return false;
	if (nfirst >= nsize) return false;
	if (place >= pow(2, psize + nsize - pfirst - nfirst)) return false; // already reaches the last one element....
	for (int i = nsize - 1; i > nfirst; i--) {
		if (place >> (nsize - 1 - i) & 1)
			nglabel[i] = -1;
		else
			nglabel[i] = 1;
	}
	nglabel[nfirst] = -1;
	for (int i = nfirst - 1; i >= 0; i--) {
		nglabel[i] = 1;
	}

	int nlength = nsize - nfirst;
	for (int i = psize - 1; i > pfirst; i--) {
		if (place >> (psize - 1 - i + nlength) & 1)
			pglabel[i] = 1;
		else
			pglabel[i] = -1;
	}
	pglabel[pfirst] = 1;
	for (int i = nfirst - 1; i >= 0; i--) {
		pglabel[i] = -1;
	}
	return true;
}


class Cluster
{
	private:
		double (*pdata)[VARS];
		double (*ndata)[VARS];
		//double* label;
		int* pg; //group No. for each elements
		int* ng; //group No. for each elements
		int* pgid;
		int* ngid;

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
		bool classify(std::vector<Equation>& X)
		{
			if (pgid == NULL) pgid = new int[pncluster];
			if (ngid == NULL) ngid = new int[nncluster];
			double** train_data = new double*[psize + nsize];
			double* train_label = new double[psize + nsize];
			int idx = 0;
			for(int i = 0; i < psize; i++) {
				train_data[idx++] = pdata[i];
			}
			for(int i = 0; i < nsize; i++) {
				train_data[idx++] = ndata[i];
			}
			problem.l = idx;
			problem.x = (svm_node**)train_data;
			problem.y = train_label;
			Equation* equation = new Equation();

			for (int pi = 0; pi < pncluster; pi++) {
				for (int ni = 0; ni < nncluster; ni++) {
					int current_place = 0;
					// pi, ni is the first element to keep current label in groups
					// compose label info for data in all groups
					while (groupLabelRandomization(pgid, ngid, pi, ni, pncluster, nncluster, current_place)) {
						current_place++;
						for (int i = 0; i < psize; i++) {
							train_label[i] = pgid[pg[i]];
						}
						for (int i = 0; i < nsize; i++) {
							train_label[i+psize] = ngid[ng[i]];
						}

						// do SVM learning
						model = svm_train(&problem, &param);
						svm_model_visualization(model, *equation);
						svm_free_and_destroy_model(&model);

						// check perfectly classify or not...
						int pass = 0;
						for (int i = 0; i < problem.l; i++) {
							pass += (Equation::calc(*equation, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
						}

						// can not classify perfectly
						if (pass < problem.l)
							continue;

						// if pass check, add classify to X set.
						// break;
						X.push_back(*equation);
						int nclassify = 0;
						for (int ppi = 0; ppi < pncluster; ppi++) {
							for (int nni = 0; nni < nncluster; nni++) {
								bool bcanclassify = false;
								for (int i = 0; i < (int)X.size(); i++) {
									if(pairSeparableByEquation(ppi, nni, X[i]) == true) {
										bcanclassify = true;
										nclassify++;
										break;
									}
								}
								if (bcanclassify == true) continue;
								// find a pair ppi and nni, which can not be classified currently....
								if (bcanclassify == false) break;
							}
							if (nclassify < ppi * nncluster) break;
						}
						if (nclassify < pncluster * nncluster) continue;
						break;
					}
				}
			}
			delete equation;
			delete []train_data;
			delete []train_label;
			return true;
		}

		Cluster(double (*pdata)[VARS], double (*ndata)[VARS], int psize, int nsize){
			this->pdata = pdata;
			this->ndata = ndata;
			pg = new int[psize];
			ng = new int[nsize];
			pgid = NULL;
			ngid = NULL;
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
			if (pg != NULL) delete []pg;
			if (ng != NULL) delete []ng;
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
			int* g = pg;
			int cursize = psize;
			int curcluster = pncluster;
			if (!positive) {
				g = ng;
				cursize = nsize;
				curcluster = nncluster;
			}
			for (int m = 0; m < cursize; m++) {
				if (g[m] == i2)
					g[m] = i1;
				else if (g[m] == curcluster)
					g[m] = i2;
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
			int* g;
			if (positive) {
				data = pdata;
				g = pg;
			} else {
				data = ndata;
				g = ng;
			}
			if (k > max_size) return -1;
			if (k > size) return -1;
			if (k == size) {
				for (int i = 0; i < k; i++)
					g[i] = i;
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

					if (g[i] != min_center) {
						regrouped = true;
						g[i] = min_center;
					}
				}
				if (regrouped == false)
					break;

				// recalculate means for each group
				for (int i = 0; i < k; i++)
					g_members[i] = 0;
				for (int i = 0; i < size; i++) {
					g_members[g[i]]++;
					for (int j = 0; j < VARS; j++)
						center[g[i]][j] += data[i][j];
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

		bool pairSeparableByEquation(int p_gid, int n_gid, Equation& eq)
		{
			for(int i = 0; i < psize; i++) {
				if (pg[i] == p_gid) {
					if(Equation::calc(eq, (double*)pdata[i]) * 1 < 0)
						return false;
				}
			}
			for(int i = 0; i < nsize; i++) {
				if (ng[i] == n_gid) {
					if(Equation::calc(eq, (double*)ndata[i]) * (01) < 0)
						return false;
				}
			}

			return true;
		}

		bool pairLinearSeparable(int p_gid, int n_gid)
		{
			double** train_data = new double*[size];
			double* train_label = new double[size];
			int idx = 0;
			for(int i = 0; i < psize; i++) {
				if (pg[i] == p_gid) {
					train_data[idx] = pdata[i];
					train_label[idx++] = 1;
				}
			}
			for(int i = 0; i < nsize; i++) {
				if (ng[i] == n_gid) {
					train_data[idx] = ndata[i];
					train_label[idx++] = -1;
				}
			}
			problem.l = idx;
			problem.x = (svm_node**)train_data;
			problem.y = train_label;

			model = svm_train(&problem, &param);
			Equation* equation = new Equation();
			svm_model_visualization(model, *equation);
			svm_free_and_destroy_model(&model);

			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (Equation::calc(*equation, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
			}

			delete equation;
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
			int *pair_gid = pg;
			int pair_label = 1;
			int pair_size = psize;
			double (*set_data)[VARS] = ndata;
			int set_label = -1;
			int set_size = nsize;
			if (!positive) {
				pair_data = ndata;
				pair_gid = ng;
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

			model = svm_train(&problem, &param);
			Equation* equation = new Equation();
			svm_model_visualization(model, *equation);
			svm_free_and_destroy_model(&model);

			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (Equation::calc(*equation, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
			}

			delete equation;
			delete []train_data;
			delete []train_label;
			if (pass < problem.l)
				return false;
			return true;
		}
};

#endif /* _SVM_H */
