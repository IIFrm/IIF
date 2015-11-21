#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"
#include "iif_svm_learn.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

IIF_svm_learn::IIF_svm_learn(States* gsets, int (*func)(int*)) : IIF_learn(gsets, func) { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
}

IIF_svm_learn::IIF_svm_learn() : IIF_learn() { 
	svm = new SVM(print_null);
	svm->main_equation = NULL;
}


int IIF_svm_learn::learn()
{
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool b_similar_last_time = false;
	bool b_converged = false;
	bool b_svm_i = false;
	Equation* previous_equations;
	int pre_positive_size = 0, pre_negative_size = 0; // , pre_question_size = 0;
	int cur_positive_size = 0, cur_negative_size = 0; // , cur_question_size = 0;
	int pre_question_index = 0;

	previous_equations = new Equation[1];
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iter) && (pass_rate >= 1)); rnd++) {
init_svm:
		std::cout << "[" << rnd << "]SVM-----------------------------------------------" << "-------------------------------------------------------------" << std::endl;
		if (rnd == 1) {
			/*
			 *	The first round is very special, so we put this round apart with its following rounds.
			 *	1> We used random values as inputs for program executions in the first round.
			 *	2> We need to make sure there are at last two classes of generated traces. "positive" and "negative"
			 */
			std::cout << "\t(1) execute programs... [" << init_exes + random_exes << "] {Random";
			for (int i = 0; i < init_exes + random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
				std::cout << inputs;
				if (i < init_exes + random_exes - 1) std::cout << "|";
				run_target(inputs);
			}
			std::cout << "}" << std::endl;

			if (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0) {
				if (gsets[POSITIVE].traces_num() == 0) std::cout << "[0] Positive trace, execute program again." << std::endl;
				if (gsets[NEGATIVE].traces_num() == 0) std::cout << "[0] Negative trace, execute program again." << std::endl;
				goto init_svm;
			}
		}
		else {
check_in_svm_step1:
			std::cout << "\t(1) execute programs...[" << (after_exes + random_exes) << "] {Random";
			for (int i = 0; i < random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
				std::cout << inputs;
				std::cout << "|";
				run_target(inputs);
			}
			std::cout << "Solve";
			for (int i = 0; i < after_exes; i++) {
				Equation::linear_solver(&previous_equations[0], inputs);
				std::cout << "|" << inputs;
				run_target(inputs);
			}
			std::cout << "}" << std::endl;

		}


svm_step2:
		std::cout << "\t(2) prepare training data... ";
		svm->prepare_training_data(gsets, pre_positive_size, pre_negative_size);
		std::cout << std::endl;

		std::cout << "\t(3) start training... ";
		svm->train();
		std::cout << "|-->> ";
		set_console_color(std::cout);
		std::cout << *svm << std::endl;
		unset_console_color(std::cout);



		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
		std::cout << "\t(4) checking training traces.";
		pass_rate = svm->predict_on_training_set();

		if (pass_rate == 1) set_console_color(std::cout, GREEN);
		else set_console_color(std::cout, RED);
		std::cout << " [" << pass_rate * 100 << "%]";
		unset_console_color(std::cout);

		if (pass_rate < 1) {
			std::cout << " [FAIL] \n The problem is not linear separable.. Trying to solve is by SVM-I algo" << std::endl;
			//std::cerr << "*******************************USING SVM_I NOW******************************" << std::endl;
			break;
		}
		set_console_color(std::cout, GREEN);
		std::cout << " [PASS]" << std::endl;
		unset_console_color(std::cout);


		/*
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
		std::cout << "\t(5) checking question traces.";

		set_console_color(std::cout, GREEN);
		if (svm->check_question_set(gsets[QUESTION]) != 0) {
			set_console_color(std::cout, RED);
			std::cout << std::endl << "check on question set return error." << std::endl;
			unset_console_color(std::cout);
			return -1;
		}
		unset_console_color(std::cout);
		std::cout << std::endl;


		/*
		 *	b_similar_last_time is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
		std::cout << "\t(6) check convergence:        ";
		if (svm->get_converged(previous_equations, 1) == 0) {
			if (b_similar_last_time == true) {
				std::cout << "[TT]  [SUCCESS] rounding off" << std::endl;
				b_converged = true;
				break;
			}
			std::cout << "[FT]";
			b_similar_last_time = true;
		}
		else {
			std::cout << ((b_similar_last_time == true) ? "[T" : "[F") << "F] ";
			b_similar_last_time = false;
		}
		std::cout << "  [FAIL] neXt round " << std::endl;

		previous_equations[0] = *(svm->main_equation);
		delete svm->main_equation;
	} // end of SVM training procedure


	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm for " << rnd << " times." << std::endl;

	int ret = 0;
	if ((b_converged) && (rnd <= max_iter)) {
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

	if ((pass_rate < 1) || (rnd >= max_iter)) {
		set_console_color(std::cout, RED);
		std::cout << "  Cannot divide by SVM perfectly.\n";
		unset_console_color(std::cout);
		ret = -1;
	}

	delete svm;
	delete previous_equations;

	return ret;
}

