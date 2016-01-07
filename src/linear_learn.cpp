#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"
#include "linear_learn.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

LinearLearn::LinearLearn(States* gsets, int (*func)(int*), int max_iteration) : LearnBase(gsets, func) { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
	this->max_iteration = max_iteration; 
}

LinearLearn::LinearLearn() : LearnBase() { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
	this->max_iteration = max_iter; 
}


int LinearLearn::learn()
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
		svm->makeTrainingSet(gsets, pre_positive_size, pre_negative_size);

#ifdef __PRT
		std::cout << "\n\t(3) start training... ";
#endif
		svm->train();
		std::cout << "|-->> ";
		setColor(std::cout, YELLOW);
		std::cout << *svm << std::endl;
		setColor(std::cout);



		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(4) checking training traces.";
#endif
		pass_rate = svm->checkTrainingSet();

#ifdef __PRT
		if (pass_rate == 1) setColor(std::cout, GREEN);
		else setColor(std::cout, RED);
		std::cout << " [" << pass_rate * 100 << "%]";
		setColor(std::cout);
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
		setColor(std::cout, GREEN);
		std::cout << " [PASS]" << std::endl;
		setColor(std::cout);
#endif


		/*
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
		/*
#ifdef __PRT
		std::cout << "\t(5) checking question traces.";
#endif

#ifdef __PRT
		setColor(std::cout, GREEN);
#endif
		if (svm->check_question_set(gsets[QUESTION]) != 0) {
#ifdef __PRT
			setColor(std::cout, RED);
			std::cout << std::endl << "check on question set return error." << std::endl;
			setColor(std::cout);
#endif
			return -1;
		}
#ifdef __PRT
		setColor(std::cout);
		std::cout << std::endl;
#endif
*/


		/*
		 *	b_similar_last_time is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\t(5) check convergence:        ";
#endif
		if (svm->converged(previous_equations, 1) == 0) {
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
		setColor(std::cout, YELLOW);
		std::cout << "  Hypothesis Invairant(Converged): {\n";
		std::cout << "\t\t" << equs[0] << std::endl;
		std::cout << "  }" << std::endl;
		setColor(std::cout);
		delete[]equs;
	}

	if ((pass_rate < 1) || (rnd >= max_iteration)) {
		setColor(std::cout, RED);
		std::cout << "  Cannot divide by SVM perfectly.\n";
		setColor(std::cout);
		ret = -1;
	}

	delete svm;
	delete previous_equations;

	return ret;
}

