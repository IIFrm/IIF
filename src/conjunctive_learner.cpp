#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"
#include "conjunctive_learner.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

ConjunctiveLearner::ConjunctiveLearner(States* gsets, int (*func)(int*), int max_iteration) : BaseLearner(gsets, func) { 
	svm_i = new SVM_I(print_null);
	this->max_iteration = max_iteration;
}


ConjunctiveLearner::~ConjunctiveLearner() { 
	if (svm_i != NULL)
		delete svm_i;
}


/// type == 0, solve equations defined by paras....
//			   if paras == NULL, all are random points
/// type > 0, params contains type number of inputs.....
/// type < 0, params contains the CLassifiers obtained last time.
//			   if params != NULL, selective sampling
//			   -type specifies the number of equations
int ConjunctiveLearner::selectiveSampling(int randn, int exen, int type, void* params)
{
	if (type == 0) {
		randn += exen;
		exen = 0;
	} else if ((type > 0) && (exen > type)) {
		randn += exen - type;
		exen = type;
	} else if (type < 0) {
		if (exen + type > 0) {
			randn += exen % (-type);
			exen = exen / (-type) * (-type);
		} else {
			randn += exen + type;
			exen = -type;
		}
	}
	
#ifdef __PRT
	std::cout << "{";
#endif
	Solution input;
	for (int i = 0; i < randn; i++) {
		Equation::linear_solver(NULL, input);
#ifdef __PRT
		std::cout << input << "|";
#endif
		runTarget(input);
	}
	if (exen == 0) {
#ifdef __PRT
		std::cout << "}" << std::endl;
#endif
		return randn + exen;
	}

#ifdef __PRT
	std::cout << "+|";
#endif

	if (type < 0) {
		int nclassifier = -type;
		int neach = exen / nclassifier;
		Equation* p = (Equation*)params;
		for (int i = 0; i < nclassifier; i++) {
			for (int j = 0; j < neach; j++) {
				Equation::linear_solver(&p[i], input);
#ifdef __PRT
				std::cout << input << "|";
#endif
				runTarget(input);
			}
		}
	} else if (type > 0) {
		Solution* p = (Solution*)params;
		for (int i = 0; i < type; i++) {
#ifdef __PRT
			std::cout << p[i] << "|";
#endif
			runTarget(p[i]);
			//runTarget(*(((Solution*)paras)+i));
		}
	}
#ifdef __PRT
	std::cout << "}" << std::endl;
#endif
	return randn + exen;
}


int ConjunctiveLearner::learn()
{
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool lastSimilar = false;
	bool converged = false;
	Equation* pre_classifiers = NULL;
	int pre_csf_num = 0;
	int pre_psize = 0, pre_nsize = 0; // , pre_question_size = 0;
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
init_svm_i:
		std::cout << "[" << rnd << "]";
		int exes = (rnd == 1)? init_exes : after_exes;
#ifdef __PRT
		int step = 1;
		std::cout << "SVM-I----------------------------------------------------------------------------------------------------------" << std::endl;
		std::cout << "\t(" << step++ << ") execute programs... [" << exes + random_exes << "] ";
#endif
		selectiveSampling(random_exes, exes, -pre_csf_num, pre_classifiers);

		if ((rnd == 1) && (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0)) {
#ifdef __PRT
			if (gsets[POSITIVE].traces_num() == 0) std::cout << "[0] Positive trace, execute program again." << std::endl;
			if (gsets[NEGATIVE].traces_num() == 0) std::cout << "[0] Negative trace, execute program again." << std::endl;
#endif
			goto init_svm_i;
		}

#ifdef __PRT
		std::cout << "\t(" << step++ << ") prepare training data... ";
#endif
		svm_i->makeTrainingSet(gsets, pre_psize, pre_nsize);

#ifdef __PRT
		std::cout << "\n\t(" << step++ << ") start training... ";
#endif
		int ret = svm_i->train();
		if (ret == -1) {
			std::cerr << RED << "[FAIL] ..... Can not dividey by SVM_I." << std::endl << WHITE;
			return -1;
		}
		std::cout << "|-->> " << YELLOW << *svm_i << std::endl << WHITE;

		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") checking training traces.";
#endif
		pass_rate = svm_i->checkTrainingSet();

#ifdef __PRT
		if (pass_rate == 1) std::cout << GREEN << " [" << pass_rate * 100 << "%]" << WHITE;
		else std::cout << RED << " [" << pass_rate * 100 << "%]" << WHITE;
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
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
		/*
#ifdef __PRT
		std::cout << "\t(5) checking question traces.";
#endif
		if (svm_i->checkQuestionSet(gsets[QUESTION]) != 0) {
#ifdef __PRT
			std::cout << std::endl << "check on question set return error." << std::endl;
#endif
			return -1;
		}
#ifdef __PRT
		std::cout << std::endl;
#endif
*/

		/*
		 *	lastSimilar is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") check convergence:        ";
#endif
		if (svm_i->converged(pre_classifiers, pre_csf_num) == 0) {
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

		pre_csf_num = svm_i->getClassifierNum();
		if (pre_classifiers != NULL) delete[]pre_classifiers;
		pre_classifiers = svm_i->saveClassifier();
	} // end of SVM_I training procedure


	std::cout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm_i for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((converged) && (rnd <= max_iteration)) {
		int equ_num = -1;
		Equation* equs = svm_i->roundoff(equ_num);
		std::cout << YELLOW << "  Hypothesis Invairant(Converged): {";
		std::cout << " \n\t ------------------------------------------------------";
		std::cout << YELLOW << " \n\t |     " << GREEN <<  equs[0];
		for (int i = 1; i < equ_num; i++) {
			std::cout << YELLOW << " \n\t |  " << GREEN << "/\\ " << equs[i];
		}
		std::cout << YELLOW << " \n\t ------------------------------------------------------\n" << WHITE;
		delete[]equs;
	}

	if (pre_classifiers != NULL) delete[]pre_classifiers;
	if ((pass_rate < 1) || (rnd >= max_iteration)) {
		std::cout << RED << "  Cannot divide by SVM_I perfectly.\n" << WHITE;
		ret = -1;
	}

	return ret;
}
