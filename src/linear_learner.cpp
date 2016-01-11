#include "config.h"
#include "color.h"
#include "equation.h"
#include "linear_learner.h"

#include <iostream>
//#include <float.h>
//#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

LinearLearner::LinearLearner(States* gsets, int (*func)(int*), int max_iteration) : BaseLearner(gsets, func) { 
	svm = new SVM(print_null);
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

	if (type == 0) {
		for (int i = 0; i < exen; i++) {
			Equation::linear_solver((Equation*)params, input);
#ifdef __PRT
			std::cout << input << "|";
#endif
			runTarget(input);
		}
	} else if (type > 0) {
		Solution* p = (Solution*)params;
		for (int i = 0; i < type; i++) {
#ifdef __PRT
			std::cout << p[i] << "|";
#endif
			runTarget(p[i]);
			//run_target(*(((Solution*)paras)+i));
		}
	}
#ifdef __PRT
	std::cout << "}" << std::endl;
#endif
	return randn + exen;
}


int LinearLearner::learn()
{
	int rnd;
	bool similarLast = false;
	bool converged = false;
	Equation* lastEquation = NULL;
	int pre_psize = 0, pre_nsize = 0; // , pre_question_size = 0;

	// lastEquation = new Equation[1];
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
		std::cout << "[" << rnd << "]";
		int exes = (rnd == 1)? init_exes : after_exes;
init_svm:
#ifdef __PRT
		int step = 1;
		std::cout << "SVM------------------------------------------------------------------------------------------------------------" << std::endl;
		std::cout << "\t(" << step++ << ") execute programs... [" << exes + random_exes << "] ";
#endif
		selectiveSampling(random_exes, exes, 0, (void*)lastEquation);

		if ((rnd == 1) && (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0)) {
#ifdef __PRT
			if (gsets[POSITIVE].traces_num() == 0) std::cout << "[0] Positive trace, execute program again." << std::endl;
			if (gsets[NEGATIVE].traces_num() == 0) std::cout << "[0] Negative trace, execute program again." << std::endl;
#endif
			goto init_svm;
		}

#ifdef __PRT
		std::cout << "\t(" << step++ << ") prepare training data... ";
#endif
		svm->makeTrainingSet(gsets, pre_psize, pre_nsize);


#ifdef __PRT
		std::cout << "\n\t(" << step++ << ") start training... ";
#endif
		svm->train();
		std::cout << "|-->> " << YELLOW << *svm << WHITE << std::endl;



		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") checking training traces.";
#endif
		pass_rate = svm->checkTrainingSet();

#ifdef __PRT
		if (pass_rate == 1) std::cout << GREEN << " [" << pass_rate * 100 << "%]" << WHITE;
		else std::cout <<  RED << " [" << pass_rate * 100 << "%]" << WHITE;
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
		std::cout << GREEN << " [PASS]" << WHITE;
#endif


		/*
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
		/*
#ifdef __PRT
std::cout << "\n\t(" << step++ << ") checking question traces.";
#endif
if (svm->checkQuestionSet(gsets[QUESTION]) != 0) {
#ifdef __PRT
std::cout << std::endl << RED << "check on question set return error." << std::endl << WHITE;
#endif
return -1;
}
*/

		/*
		 *	similarLast is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\n\t(" << step++ << ") check convergence:        ";
#endif
		if (svm->converged(lastEquation, 1) == 0) {
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
#endif
			similarLast = true;
		} else {
#ifdef __PRT
			std::cout << ((similarLast == true) ? "[T" : "[F") << "F] ";
#endif
			similarLast = false;
		}
#ifdef __PRT
std::cout << "  [FAIL] neXt round " << std::endl;
#endif

if (lastEquation == NULL) lastEquation = new Equation[1];
lastEquation[0] = *(svm->getClassifier());
} // end of SVM training procedure


std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
std::cout << "Finish running svm for " << rnd - 1 << " times." << std::endl;

int ret = 0;
if ((converged) && (rnd <= max_iteration)) {
	int equ_num = -1;
	Equation* equ = svm->roundoff(equ_num);
	assert(equ_num == 1);
	std::cout << YELLOW << "  Hypothesis Invairant(Converged): {\n";
	std::cout << "\t\t" << GREEN << *equ << YELLOW << std::endl;
	std::cout << "  }" << WHITE << std::endl;
	delete equ;
}

if ((pass_rate < 1) || (rnd >= max_iteration)) {
	std::cout << RED << "  Cannot divide by SVM perfectly.\n" << WHITE;
	ret = -1;
}

if (lastEquation) delete lastEquation;

return ret;
}

std::string LinearLearner::invariant() {
	int equ_num = -1;
	Equation* equ = svm->roundoff(equ_num);
	assert(equ_num == 1);
	std::string s = equ->toString();
	delete equ;
	return s;
}
