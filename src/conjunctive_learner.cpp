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
//			   if paras != NULL, selective sampling
/// type > 0, paras contains type number of inputs.....
int ConjunctiveLearner::programExecutor(int randn, int exen, int type, void* paras)
{
	if ((type != 0) && (exen > type))
		randn += exen - type;
#ifdef __PRT
	std::cout << "{";
#endif
	Solution input;
	for (int i = 0; i < randn; i++) {
		Equation::linear_solver(NULL, input);
#ifdef __PRT
		std::cout << input << "|";
#endif
		run_target(input);
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

	if (type == 0) {
		for (int i = 0; i < exen; i++) {
			Equation::linear_solver((Equation*)paras, input);
#ifdef __PRT
			std::cout << input << "|";
#endif
			run_target(input);
		}
	} else if (type > 0) {
		Solution* p = (Solution*)paras;
		for (int i = 0; i < type; i++) {
#ifdef __PRT
			std::cout << p[i] << "|";
#endif
			run_target(p[i]);
			//run_target(*(((Solution*)paras)+i));
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
#endif
		if (rnd != 1) {
			int exes_each_equation = (after_exes + pre_csf_num - 1) / pre_csf_num;
#ifdef __PRT
			std::cout << "\t(" << step++ <<") execute programs...[" <<  (exes_each_equation * pre_csf_num + random_exes) << "] {Random";
#endif
			for (int i = 0; i < random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
#ifdef __PRT
				std::cout << inputs << "|";
#endif
				run_target(inputs);
			}

			for (int i = 0; i < svm_i->getClassifierNum(); i++) {
				for (int j = 0; j < exes_each_equation; j++) {
					Equation::linear_solver(&pre_classifiers[i], inputs);
#ifdef __PRT
					std::cout << " | " << inputs;
#endif
					run_target(inputs);
				}
			}
#ifdef __PRT
			std::cout << "}" << std::endl;
#endif
		} else {
			pre_psize = 0;
			pre_nsize = 0;
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
		int equation_num = -1;
		Equation* equs = svm_i->roundoff(equation_num);
		std::cout << YELLOW << "  Hypothesis Invairant(Converged): {";
		std::cout << " \n\t ------------------------------------------------------";
		std::cout << YELLOW << " \n\t |     " << GREEN <<  equs[0];
		for (int i = 1; i < equation_num; i++) {
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
