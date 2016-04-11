#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "polynomial.h"
#include "conjunctive_learner.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

ConjunctiveLearner::ConjunctiveLearner(States* gsets, const char* solution_filename, 
		int (*func)(int*), int max_iteration) : BaseLearner(gsets, solution_filename, func) { 
	svm_i = new SVM_I(0, print_null);
	this->max_iteration = max_iteration;
}

ConjunctiveLearner::~ConjunctiveLearner() { 
	if (svm_i != NULL)
		delete svm_i;
}

int ConjunctiveLearner::learn()
{
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool lastSimilar = false;
	bool converged = false;
	Classifier pre_cl;
	int pre_psize = 0, pre_nsize = 0; // , pre_question_size = 0;
	double pass_rate = 1;

	int zero_times = 0;

	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
		//std::cout << "[" << rnd << "]";
		int nexe = (rnd == 1) ? Nexe_init : Nexe_after;
#ifdef __PRT
		int step = 1;
		std::cout << "SVM-I----------------------------------------------------------"
			"------------------------------------------------";
		std::cout << "\n\t(" << step++ << ") execute programs... [" << nexe + Nexe_rand << "] ";
#else
		std::cout << RED << "[" << rnd;
#endif
init_svm_i:
		selectiveSampling(Nexe_rand, nexe, 0, &(pre_cl));

		if ((rnd == 1) && (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0)) {
#ifdef __PRT
			if (gsets[POSITIVE].traces_num() == 0) 
				std::cout << RED << "\tZero Positive trace, execute program again." << WHITE << std::endl;
			if (gsets[NEGATIVE].traces_num() == 0) 
				std::cout << RED << "\tZero Negative trace, execute program again." << WHITE << std::endl;
#else
			std::cout << "+";
#endif
			zero_times++;
			if (zero_times >= Nretry_init)
				exit(-1);
			goto init_svm_i;
		}

#ifdef __PRT
		std::cout << "\t(" << step++ << ") prepare training data... ";
#else
		std::cout << "]" << WHITE;
#endif
		svm_i->makeTrainingSet(gsets, pre_psize, pre_nsize);

#ifdef __PRT
		std::cout << "\n\t(" << step++ << ") start training... ";
#endif
#ifdef __DS_ENABLED
		std::cout << "[" << svm_i->problem.np << ":" << svm_i->negative_size << "]";
#endif
		int ret = svm_i->train();
		if (ret == -1) {
			std::cerr << RED << "[FAIL] ..... Can not dividey by SVM_I." << std::endl << WHITE;
			return -1;
		}
		//svm_i->cl.roundoff();
		std::cout << "|-->> " << YELLOW << *svm_i << std::endl << WHITE;

		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") checking training traces.";
#else
		//std::cout << "]" << WHITE;
#endif
		pass_rate = svm_i->checkTrainingSet();

#ifdef __PRT
		if (pass_rate == 1) 
			std::cout << GREEN << " [" << pass_rate * 100 << "%]" << WHITE;
		else 
			std::cout << RED << " [" << pass_rate * 100 << "%]" << WHITE;
#endif

		if (pass_rate < 1) {
			std::cerr << RED << "[FAIL] ..... Can not dividey by SVM_I." << std::endl << WHITE;
			rnd++;
			break;	
		}
#ifdef __PRT
		std::cout << GREEN << " [PASS]" << std::endl << WHITE;
#endif

		/*
		 *	lastSimilar is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") check convergence:        ";
#endif
		if (svm_i->converged(pre_cl) == true) {
			if (lastSimilar == true) {
#ifdef __PRT
				std::cout << "[TT]  [SUCCESS] rounding off" << std::endl;
#endif
				converged = true;
				rnd++;
				break;
			}
#ifdef __PRT
			std::cout << "[FT]";
#endif
			lastSimilar = true;
		} else {
#ifdef __PRT
			std::cout << ((lastSimilar == true) ? "[T" : "[F") << "F] ";
#endif
			lastSimilar = false;
		}
#ifdef __PRT
		std::cout << "  [FAIL] neXt round " << std::endl;
#endif

		pre_cl = svm_i->cl;
		svm_i->cl.clear();
	} // end of SVM_I training procedure


	std::cout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm_i for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((converged) && (rnd <= max_iteration)) {
		svm_i->cl.roundoff();
		std::cout << YELLOW << "  Hypothesis Invairant(Conjunctive): { ";
		std::cout << GREEN << svm_i->cl << YELLOW;
		std::cout << " }" << WHITE << std::endl;
	}

	if ((pass_rate < 1) || (rnd > max_iteration)) {
		std::cout << RED << "  Cannot divide by SVM_I perfectly.\n" << WHITE;
		std::cout << pass_rate << "\t" << rnd << std::endl;
		ret = -1;
	}

	//std::cout << "return value = " << ret << std::endl;
	return ret;
}


std::string ConjunctiveLearner::invariant(int n) {
	svm_i->cl.roundoff();
	return svm_i->cl.toString();
}