#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"
#include "iif_disjunctive_svm_learn.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

IIF_disjunctive_svm_learn::IIF_disjunctive_svm_learn(States* gsets, int (*func)(int*), int max_iteration, int K) : IIF_learn(gsets, func) { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
	this->max_iteration = max_iteration; 
	this->K = K;
	/*
	positive_group = new int[gsets[1].size()];
	negative_group = new int[gsets[-1].size()];
	checkset = new double[gsets[-1].size() + gsets[1].size()][VARS];
	*/
}

IIF_disjunctive_svm_learn::IIF_disjunctive_svm_learn() : IIF_learn() { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
	this->max_iteration = max_iter; 
	this->K = 8;
	/*
	positive_group = NULL;
	negative_group = NULL;
	checkset = NULL;
	*/
}

static double square_distance(double* a1, double* b1, int size)
{
	double distance = 0;
	for (int i = 0; i < size; i++)
		distance += (a1[i] - b1[i]) * (a1[i] - b1[i]);
	return distance;
}


int IIF_disjunctive_svm_learn::k_means(int gset_index, int k)
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



int IIF_disjunctive_svm_learn::learn()
{
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool b_similar_last_time = false;
	bool b_converged = false;
	Equation* previous_equations;
	int pre_positive_size = 0, pre_negative_size = 0; // , pre_question_size = 0;

	previous_equations = new Equation[1];
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
init_svm:
		std::cout << "[" << rnd << "]";
#ifdef __PRT
		std::cout << "SVM-----------------------------------------------" << "-------------------------------------------------------------" << std::endl;
#endif
		if (rnd == 1) {
			/*
			 *	The first round is very special, so we put this round apart with its following rounds.
			 *	1> We used random values as inputs for program executions in the first round.
			 *	2> We need to make sure there are at last two classes of generated traces. "positive" and "negative"
			 */
#ifdef __PRT
			std::cout << "\t(1) execute programs... [" << init_exes + random_exes << "] {Random";
#endif
			for (int i = 0; i < init_exes + random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
#if (__PRT || __PRT_TRACE)
				std::cout << inputs;
				if (i < init_exes + random_exes - 1) std::cout << "|";
#endif
				run_target(inputs);
			}
#ifdef __PRT
			std::cout << "}" << std::endl;
#endif

			if (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0) {
#ifdef __PRT
				if (gsets[POSITIVE].traces_num() == 0) std::cout << "[0] Positive trace, execute program again." << std::endl;
				if (gsets[NEGATIVE].traces_num() == 0) std::cout << "[0] Negative trace, execute program again." << std::endl;
#endif
				goto init_svm;
			}
		} else {
#ifdef __PRT
			std::cout << "\t(1) execute programs...[" << (after_exes + random_exes) << "] {Random";
#endif
			for (int i = 0; i < random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
#ifdef __PRT
				std::cout << inputs;
				std::cout << "|";
#endif
				run_target(inputs);
			}
#ifdef __PRT
			std::cout << "Solve";
#endif
			for (int i = 0; i < after_exes; i++) {
				Equation::linear_solver(&previous_equations[0], inputs);
#ifdef __PRT
				std::cout << "|" << inputs;
#endif
				run_target(inputs);
			}
#ifdef __PRT
			std::cout << "}" << std::endl;
#endif
		}


#ifdef __PRT
		std::cout << "\t(2) prepare training data... ";
#endif
		svm->prepare_training_data(gsets, pre_positive_size, pre_negative_size);

#ifdef __PRT
		std::cout << "\n\t(3) start training... ";
#endif
		svm->train();
		std::cout << "|-->> ";
		set_console_color(std::cout);
		std::cout << *svm << std::endl;
		unset_console_color(std::cout);



		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(4) checking training traces.";
#endif
		pass_rate = svm->predict_on_training_set();

#ifdef __PRT
		if (pass_rate == 1) set_console_color(std::cout, GREEN);
		else set_console_color(std::cout, RED);
		std::cout << " [" << pass_rate * 100 << "%]";
		unset_console_color(std::cout);
#endif

		if (pass_rate < 1) {
#ifdef __PRT
			std::cout << " [FAIL] \n The problem is not linear separable.. Trying to solve is by SVM-I algo" << std::endl;
#endif
			//std::cerr << "*******************************USING SVM_I NOW******************************" << std::endl;
			rnd++;
			break;
		}
#ifdef __PRT
		set_console_color(std::cout, GREEN);
		std::cout << " [PASS]" << std::endl;
		unset_console_color(std::cout);
#endif


		/*
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
#ifdef __PRT
		std::cout << "\t(5) checking question traces.";
#endif

#ifdef __PRT
		set_console_color(std::cout, GREEN);
#endif
		if (svm->check_question_set(gsets[QUESTION]) != 0) {
#ifdef __PRT
			set_console_color(std::cout, RED);
			std::cout << std::endl << "check on question set return error." << std::endl;
			unset_console_color(std::cout);
#endif
			return -1;
		}
#ifdef __PRT
		unset_console_color(std::cout);
		std::cout << std::endl;
#endif


		/*
		 *	b_similar_last_time is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\t(6) check convergence:        ";
#endif
		if (svm->get_converged(previous_equations, 1) == 0) {
			if (b_similar_last_time == true) {
#ifdef __PRT
				std::cout << "[TT]  [SUCCESS] rounding off" << std::endl;
#endif
				b_converged = true;
				rnd++;
				break;
			}
#ifdef __PRT
			std::cout << "[FT]";
#endif
			b_similar_last_time = true;
		}
		else {
#ifdef __PRT
			std::cout << ((b_similar_last_time == true) ? "[T" : "[F") << "F] ";
#endif
			b_similar_last_time = false;
		}
#ifdef __PRT
		std::cout << "  [FAIL] neXt round " << std::endl;
#endif

		previous_equations[0] = *(svm->main_equation);
		delete svm->main_equation;
	} // end of SVM training procedure


	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((b_converged) && (rnd <= max_iteration)) {
		int equation_num = -1;
		Equation* equs = svm->roundoff(equation_num);
		assert(equation_num == 1);
		set_console_color(std::cout, YELLOW);
		std::cout << "  Hypothesis Invairant(Converged): {\n";
		std::cout << "\t\t" << equs[0] << std::endl;
		std::cout << "  }" << std::endl;
		unset_console_color(std::cout);
		delete[]equs;
	}

	if ((pass_rate < 1) || (rnd >= max_iteration)) {
		set_console_color(std::cout, RED);
		std::cout << "  Cannot divide by SVM perfectly.\n";
		unset_console_color(std::cout);
		ret = -1;
	}

	delete svm;
	delete previous_equations;

	return ret;
}

