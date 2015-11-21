#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"
#include "iif_learn.h"
#include "iif_svm_i_learn.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

IIF_svm_i_learn::IIF_svm_i_learn(States* gsets, int (*func)(int*)) : IIF_learn(gsets, func) { 
	svm_i = new SVM_I(print_null);
}

IIF_svm_i_learn::IIF_svm_i_learn() : IIF_learn() { 
	svm_i = new SVM_I(print_null);
}


int IIF_svm_i_learn::learn()
{
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool b_similar_last_time = false;
	bool b_converged = false;
	Equation* previous_equations;
	int pre_positive_size = 0, pre_negative_size = 0; // , pre_question_size = 0;
	int cur_positive_size = 0, cur_negative_size = 0; // , cur_question_size = 0;
	int pre_question_index = 0;

	previous_equations = new Equation[1];
	int pre_equation_num = 0;
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iter) && (pass_rate >= 1)); rnd++) {
		//	init_svm_i:
		std::cout << "[" << rnd << "]SVM-I---------------------------------------------" << "-------------------------------------------------------------" << std::endl;
		if (rnd != 1) {
			int exes_each_equation = (after_exes + pre_equation_num - 1) / pre_equation_num;
			std::cout << "\t(1) execute programs...[" <<  (exes_each_equation * pre_equation_num + random_exes) << "] {Random";

			for (int i = 0; i < random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
				std::cout << inputs << "|";
				run_target(inputs);
			}

			for (int i = 0; i < svm_i->equ_num; i++) {
				for (int j = 0; j < exes_each_equation; j++) {
					Equation::linear_solver(&previous_equations[i], inputs);
					std::cout << " | " << inputs;
					run_target(inputs);
				}
			}
			std::cout << "}" << std::endl;
		} else {
			pre_positive_size = 0;
			pre_negative_size = 0;
		}

svm_i_step2:
		std::cout << "\t(2) prepare training data... ";
		svm_i->prepare_training_data(gsets, pre_positive_size, pre_negative_size);
		std::cout << std::endl;

		std::cout << "\t(3) start training... ";
		int ret = svm_i->train();
		if (ret == -1)
			return -1;
		//std::cout << svm_i->equ_num;
		std::cout << "|-->> ";
		set_console_color(std::cout);
		std::cout << *svm_i << std::endl;
		unset_console_color(std::cout);

		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
		std::cout << "\t(4) checking training traces.";
		double passRat = svm_i->predict_on_training_set();

		if (passRat == 1) set_console_color(std::cout, GREEN);
		else set_console_color(std::cout, RED);
		std::cout << " [" << passRat * 100 << "%]";
		unset_console_color(std::cout);

		if (passRat < 1) {
			set_console_color(std::cout, RED);
			std::cerr << "[FAIL] ..... Can not dividey by SVM_I." << std::endl;
			//std::cerr << "[FAIL] ..... Reaching maximium num of equation supported by SVM_I." << std::endl;
			//std::cerr << "You can increase the limit by modifying [classname::methodname]=SVM-I::SVM-I(..., int equ = **) " << std::endl;
			unset_console_color(std::cout);
			//return -1;
			//			b_svm_i = true;
			//			break;
		}
		set_console_color(std::cout, GREEN);
		std::cout << " [PASS]" << std::endl;
		unset_console_color(std::cout);


		/*
		 *	Check on Question traces.
		 *	There should not exists one traces, in which a negative state is behind a positive state.
		 */
		std::cout << "\t(5) checking question traces.";
		if (svm_i->check_question_set(gsets[QUESTION]) != 0) {
			std::cout << std::endl << "check on question set return error." << std::endl;
			return -1;
		}
		std::cout << std::endl;


		/*
		 *	b_similar_last_time is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
		std::cout << "\t(6) check convergence:        ";
		if (svm_i->get_converged(previous_equations, pre_equation_num) == 0) {
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


		pre_equation_num = svm_i->equ_num;
		delete[]previous_equations;
		previous_equations = new Equation[pre_equation_num];
		for (int i = 0; i < svm_i->equ_num; i++) {
			previous_equations[i] = svm_i->equations[i];
		}

		std::cout << std::endl;
	} // end of SVM_I training procedure


	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm_i for " << rnd << " times." << std::endl;

	int ret = 0;
	if ((b_converged) && (rnd <= max_iter)) {
		int equation_num = -1;
		Equation* equs = svm_i->roundoff(equation_num);
		set_console_color(std::cout, YELLOW);
		std::cout << "  Hypothesis Invairant(Converged): {\n";
		std::cout << " \n\t ------------------------------------------------------";
		std::cout << " \n\t |     " << equs[0];
		for (int i = 1; i < equation_num; i++) {
			std::cout << " \n\t |  /\\ " << equs[i];
		}
		std::cout << " \n\t ------------------------------------------------------";
		unset_console_color(std::cout);
		delete[]equs;
	}

	if ((pass_rate < 1) || (rnd >= max_iter)) {
		set_console_color(std::cout, RED);
		std::cout << "  Cannot divide by SVM_I perfectly.\n";
		unset_console_color(std::cout);
		ret = -1;
	}

	delete []previous_equations;
	delete svm_i;

	return ret;
}
