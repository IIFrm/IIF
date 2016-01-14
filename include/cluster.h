#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "ml_algo.h"
#include "svm_core.h"
#include "string.h"

static double sqrDistance(double* a1, double* b1, int size)
{
	double distance = 0;
	for (int i = 0; i < size; i++)
		distance += (a1[i] - b1[i]) * (a1[i] - b1[i]);
	return distance;
}



class Cluster: public MLalgo
{
	private:
		svm_parameter param;
		svm_problem problem;
		svm_model* model;
		Equation* classifiers;
		int csf_num;

		int max_size;
		int max_csf;

		double** data; // [max_items * 2];
		double* label; // [max_items * 2];
		int* gid; // group label

		int p_num;
		int n_num;
		int size;

		int K;
		int k;

	public:
		inline int getClassifierNum() {
			return csf_num;
		}

		inline Equation* getClassifier() {
			return classifiers;
		}

		inline Equation* getClassifier(int idx) {
			if (idx >= csf_num) return NULL;
			return &classifiers[idx];
		}

		inline Equation* saveClassifier() {
			Equation* es = new Equation[csf_num];
			for (int i = 0; i < csf_num; i++)
				es[i] = classifiers[i];
			return es;
		}

		Cluster(void(*f) (const char*) = NULL, int size = 10000, int csf = 16) : max_size(size), max_csf(csf) {
			prepare_linear_parameters(param);
			if (f != NULL)
				svm_set_print_string_function(f);
			model = NULL;
			classifiers = new Equation[max_csf];

			data = new double*[max_size];
			label = new double[max_size];
			gid = new int[max_size];
			for (int i = 0; i < max_size; i++)
				label[i] = -1;
			problem.l = 0;
			problem.x = (svm_node**)(data);
			problem.y = label;

			//negatives = NULL;
			csf_num = 0;
		}

		~Cluster() {
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (classifiers != NULL) delete []classifiers;
			if (data != NULL) delete []data;
			if (label != NULL) delete []label;
			if (gid != NULL) delete []gid;
		}


		virtual int makeTrainingSet(States* gsets, int& pre_positive_size, int& pre_negative_size) {
			return 0;
		}

		int train() {
			return 0;
		}

		double checkTrainingSet() {
			return 0;
		}

		int checkQuestionSet(States& qset) {
			return 0;
		}

		int converged (Equation* previous_equation, int equation_num) {
			return 0;
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

		Equation* roundoff(int& num) {
			num = 1;
			return NULL;
		}

		virtual int predict(double* v) {
			if (v == NULL) return -2;
			return -1;
		}

	private:
		int k_means(double (*data)[VARS], double* gid, int size, int k)
		{
			if (k > K)
				return -1;
			if (k > size)
				return -1;
			if (k == size) {
				for (int i = 0; i < k; i++)
					gid[i] = i;
				return 0;
			}

			double** center = new double*[k];
			double* distance = new double[k];
			int* g_members = new int[k];

			// randomly choose points as the initial means to begin K-means algorithm
			int duration = size / k;
			for (int last = 0, i = 0; i < k; i++) {
				last += rand() % duration + 1;
				center[i] = data[last];
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


			delete []g_members;
			delete []distance;
			delete []center;
			return 0;
		}

		bool linearSeparable(int p_gid, int n_gid)
		{
			double** train_data = new double*[size];
			double* train_label = new double[size];
			int idx = 0;
			for(int i = 0; i < p_num; i++) {
				if (gid[i] == p_gid) {
					train_data[idx] = data[i];
					train_label[idx++] = 1;
				}
			}
			for(int i = p_num; i < size; i++) {
				if (gid[i] == n_gid) {
					train_data[idx] = data[i];
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
			Equation* cur_csf = new Equation();
			svm_model_visualization(model, *cur_csf);
			svm_free_and_destroy_model(&model);

			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (Equation::calc(*cur_csf, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
			}

			delete []train_data;
			delete []train_label;
			if (pass < problem.l)
				return false;
			return true;
		}
	protected:
		int max_size;
};

#endif /* _SVM_H */
