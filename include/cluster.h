#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "ml_algo.h"
#include "svm_core.h"
#include "string.h"

static double square_distance(double* a1, double* b1, int size)
{
	double distance = 0;
	for (int i = 0; i < size; i++)
		distance += (a1[i] - b1[i]) * (a1[i] - b1[i]);
	return distance;
}

bool linear_separable(int positive_label, int negative_label)
{
	/*
	int index = 0;
	// copy all the data which are labeled as given to the checkset
	int positive_size = gsets[POSITIVE].size();
	int negative_size = gsets[NEGATIVE].size();
	for(int i = 0; i < positive_size; i++)
		if (positive_group[i] == positive_label)
			checkset[index++] = gsets[POSITIVE][i];
	for(int i = 0; i < negative_size; i++)
		if (negative_group[i] == negative_label)
			checkset[index++] = gsets[POSITIVE][i];
*/
	return false;
}

int k_means(int gset_index, int k)
{
	/*
	if (k > K)
		return -1;
	States& set = gsets[gset_index];
	int size = set.size();
	if (k >= size)
		return 0;
	double (*data)[VARS] = set.values;
	int* label;  
	if (gset_index == 1) label = positive_group;
	else if (gset_index == -1) label = negative_group;
	if (label == NULL)	label = new int [size]; 
	double** kmeans = new double*[size];

	// randomly choose points as the initial means to begin K-means algorithm
	for (int seed, i = 0; i < size; i++) {
		seed = rand() % k;
		kmeans[i] = new double[VARS];
		for (int j = 0; j < VARS; j++)
			kmeans[i][j] = data[seed][j];
	}


	double* distance = new double[k];
	int* group_members = new int[k];
	double total_distance = DBL_MAX;

	while (true) {	
		// regroup according to square distance.
		bool regrouped = false;
		for (int i = 0; i < size; i++) {
			int min_cand = 0;
			for (int j = 0; j < k; j++) {
				distance[j] = square_distance(kmeans[j], data[i], VARS);
				if (distance[j] < distance[min_cand])
					min_cand = j;
			}
			if (!regrouped && label[i] != min_cand)
				regrouped = true;
		}
		if (regrouped == false)
			break;

		// recalculate means for each group
		for (int i = 0; i < k; i++)
			group_members[i] = 0;
		for (int i = 0; i < size; i++) {
			group_members[label[i]]++;
			for (int j = 0; j < VARS; j++)
				kmeans[label[i]][j] += data[i][j];
		}
	}

	//for (int i = 0; i < s.size(); i++);

	delete []group_members;
	delete []distance;
	for (int i = 0; i < k; i++)
		delete []kmeans[i];
	delete []kmeans;
	delete []label;
	*/
	return 0;
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

		double* label; // [max_items * 2];
		double** data; // [max_items * 2];
		double* glabel; // group label

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
			glabel = new double[max_size];
			for (int i = 0; i < max_size; i++)
				label[i] = -1;
			problem.l = 0;
			problem.x = (svm_node**)(data);
			problem.y = label;

			negatives = NULL;
			csf_num = 0;
		}

		~Cluster() {
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (classifiers != NULL) delete []classifiers;
			if (data != NULL) delete []data;
			if (label != NULL) delete []label;
			if (glabel != NULL) delete []glabel;
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

	protected:
		int max_size;
};

#endif /* _SVM_H */
