#include "config.h"
#include "color.h"
#include "equation.h"
#include "classifier.h"
#include "linear_learner.h"

#include <iostream>
//#include <float.h>
//#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

LinearLearner::LinearLearner(States* gsets, int(*func)(int*), int max_iteration) : BaseLearner(gsets, func) {
	svm = new SVM(0, print_null);
	this->max_iteration = max_iteration;
}


LinearLearner::~LinearLearner() {
	delete svm;
}


/// type == 0, solve equations defined by paras....
//			   if params == NULL, all are random points
//			   if params != NULL, selective sampling
/// type > 0, params contains type number of inputs.....
int LinearLearner::selectiveSampling(int randn, int exen, int type, void* params)
{
#ifdef __PRT
	std::cout << "{" << GREEN;
#endif

	if ((type != 0) && (exen > type))
		randn += exen - type;
	Solution input;
	int ret = 0;
	for (int i = 0; i < randn; i++) {
		//Equation::linear_solver(NULL, input);
		//model_solver(NULL, input);
		Classifier::solver(NULL, input);
		ret = runTarget(input);
#ifdef __PRT
		std::cout << input;
		printRunResult(ret);
		std::cout << "|";
#endif
	}

#ifdef __PRT
	std::cout << BLUE;
#endif

	if (type == 0) {
		for (int i = 0; i < exen; i++) {
			//model_solver((svm_model*)params, input);
			Classifier::solver((Classifier*)params, input);
			ret = runTarget(input);
#ifdef __PRT
			std::cout << "|" << input;
			printRunResult(ret);
#endif
		}
	}
	/*else if (type > 0) {
		Solution* p = (Solution*)params;
#ifdef __PRT
			std::cout << "|>>";
#endif
		for (int i = 0; i < type; i++) {
#ifdef __PRT
			std::cout << p[i] << "|";
#endif
			runTarget(p[i]);
		}
	}
	*/
#ifdef __PRT
	std::cout << WHITE << "}" << std::endl;
#endif
	return randn + exen;
}


int LinearLearner::learn()
{
	int rnd;
	bool similarLast = false;
	bool converged = false;
	svm_model* lastModel = NULL;
	int pre_psize = 0, pre_nsize = 0;

	double pass_rate = 1;
	int etimes = 1;


	for (rnd = 1; ((rnd <= max_iteration) /*&& (pass_rate >= 1)*/); rnd++) {
		int nexe = (rnd == 1) ? Nexe_init : Nexe_after;
	init_svm:
#ifdef __PRT
		int step = 1;
		std::cout << RED << "[" << rnd << "]" << WHITE;
		std::cout << RED << "Linear SVM------------------------{" << etimes << "}------------------------------------------------------------------------------------" << WHITE << std::endl;
		std::cout << "\t(" << YELLOW << step++ << WHITE << ") execute programs... [" << nexe + Nexe_rand << "] ";
#else
		std::cout << RED << "[" << rnd << "]" << WHITE;
#endif
		//std::cout << std::endl << "\t-->selective sampling:\n\t";
		//selectiveSampling(random_exes, nexe, 0, (void*)lastEquation);
		//selectiveSampling(random_exes, nexe, 0, lastModel);
		selectiveSampling(Nexe_rand, nexe, 0, cl);
		//std::cout << "\t<--selective sampling:\n";

		if ((rnd == 1) && (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0)) {
#ifdef __PRT
			if (gsets[POSITIVE].traces_num() == 0) std::cout << RED << "\tZero Positive trace, execute program again." << WHITE << std::endl;
			if (gsets[NEGATIVE].traces_num() == 0) std::cout << RED << "\tZero Negative trace, execute program again." << WHITE << std::endl;
#endif
			goto init_svm;
		}

#ifdef __PRT
		std::cout << "\t(" << YELLOW << step++ << WHITE << ") prepare training data... ";
#endif
		svm->makeTrainingSet(gsets, pre_psize, pre_nsize);

		while (etimes <= 3) {
			cl->clear();
#ifdef __PRT
			std::cout << "\n\t(" << YELLOW << step++ << WHITE << ") start training with mapping dimension {" << etimes << "}...";
#endif
			svm->setEtimes(etimes);
			svm->train();
			std::cout << "|-->> " << YELLOW << *svm << WHITE << std::endl;
			cl->factor(*(svm->equ));

#ifdef __PRT
			std::cout << "\t(" << YELLOW << step++ << WHITE << ") checking training traces.";
#endif
			pass_rate = svm->checkTrainingSet();

#ifdef __PRT
			if (pass_rate == 1)
				std::cout << GREEN << " [100%] [PASS]" << WHITE;
			else
				std::cout << RED << " [" << pass_rate * 100 << "%]" << " [FAIL] \n The problem is not linear separable by mapping "
				<< etimes << ".. Trying to project to a higher space " << WHITE << std::endl;
#endif
			if (pass_rate == 1)
				break;
			etimes++;
		}
		if (etimes > 3) break;

		/*
		 *	similarLast is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\n\t(" << YELLOW << step++ << WHITE << ") check convergence:        ";
#endif

		if (svm->converged_model() == true) {
			if (similarLast == true) {
#ifdef __PRT
				std::cout << "[TT]  [SUCCESS] rounding off" << std::endl;
#endif
				converged = true;
				rnd++;
				break;
			}
#ifdef __PRT
			std::cout << "[FT]";
			Nexe_after *= 2;
#endif
			similarLast = true;
		}
		else {
#ifdef __PRT
			std::cout << ((similarLast == true) ? "[T" : "[F") << "F] ";
			if (similarLast == true)
				Nexe_after /= 2;
#endif
			similarLast = false;
		}
#ifdef __PRT
		std::cout << "  [FAIL] neXt round " << std::endl;
#endif

		//lastModel = svm->getModel();
		lastModel = svm->model;
	} // end of SVM training procedure


	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((converged) && (rnd <= max_iteration)) {
		/*bool sat =*/ //svm_model_z3(lastModel, cl);
		svm_model_visualization(lastModel, equ);
		//svm_model_approximate(lastModel, equ->getEtimes());
		//svm_problem_approximate(&svm->problem, equ->getEtimes());
		//std::cout << GREEN << "generated model" << *lastModel << std::endl << WHITE;
		std::cout << YELLOW << "  Hypothesis Invariant(Converged): {\n";
		std::cout << "\t\t" << GREEN << *equ << YELLOW << std::endl;
		std::cout << "  }" << WHITE << std::endl;
	}

	if ((pass_rate < 1) || (rnd >= max_iteration)) {
		std::cout << RED << "  Cannot divide by SVM perfectly.\n" << WHITE;
		ret = -1;
	}

	return ret;
}

std::string LinearLearner::invariant() {
	return cl->toString();
}
