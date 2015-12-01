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

IIF_svm_i_learn::IIF_svm_i_learn(States* gsets, int (*func)(int*), int max_iteration) : IIF_learn(gsets, func) { 
	svm_i = new SVM_I(print_null);
	this->max_iteration = max_iteration;
}

IIF_svm_i_learn::IIF_svm_i_learn() : IIF_learn() { 
	svm_i = new SVM_I(print_null);
	this->max_iteration = max_iter;
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

	previous_equations = new Equation[1];
	int pre_equation_num = 0;
	double pass_rate = 1;


	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
		//	init_svm_i:
		std::cout << "[" << rnd << "]";
#ifdef __PRT
		std::cout << "SVM-I---------------------------------------------" << "-------------------------------------------------------------" << std::endl;
#endif
		if (rnd != 1) {
			int exes_each_equation = (after_exes + pre_equation_num - 1) / pre_equation_num;
#ifdef __PRT
			std::cout << "\t(1) execute programs...[" <<  (exes_each_equation * pre_equation_num + random_exes) << "] {Random";
#endif
			for (int i = 0; i < random_exes; i++) {
				Equation::linear_solver(NULL, inputs);
#ifdef __PRT
				std::cout << inputs << "|";
#endif
				run_target(inputs);
			}

			for (int i = 0; i < svm_i->equ_num; i++) {
				for (int j = 0; j < exes_each_equation; j++) {
					Equation::linear_solver(&previous_equations[i], inputs);
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
			pre_positive_size = 0;
			pre_negative_size = 0;
		}

#ifdef __PRT
		std::cout << "\t(2) prepare training data... ";
#endif
		svm_i->prepare_training_data(gsets, pre_positive_size, pre_negative_size);

#ifdef __PRT
		std::cout << "\n\t(3) start training... ";
#endif
		int ret = svm_i->train();
		if (ret == -1)
		{
			set_console_color(std::cout, RED);
			std::cerr << "[FAIL] ..... Can not dividey by SVM_I." << std::endl;
			unset_console_color(std::cout);
			return -1;
		}
		//std::cout << svm_i->equ_num;
		std::cout << "|-->> ";
		set_console_color(std::cout);
		std::cout << *svm_i << std::endl;
		unset_console_color(std::cout);

		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(4) checking training traces.";
#endif
		pass_rate = svm_i->predict_on_training_set();

#ifdef __PRT
		if (pass_rate == 1) set_console_color(std::cout, GREEN);
		else set_console_color(std::cout, RED);
		std::cout << " [" << pass_rate * 100 << "%]";
		unset_console_color(std::cout);
#endif

		if (pass_rate < 1) {
			set_console_color(std::cout, RED);
			std::cerr << "[FAIL] ..... Can not dividey by SVM_I." << std::endl;
			//std::cerr << "[FAIL] ..... Reaching maximium num of equation supported by SVM_I." << std::endl;
			//std::cerr << "You can increase the limit by modifying [classname::methodname]=SVM-I::SVM-I(..., int equ = **) " << std::endl;
			unset_console_color(std::cout);
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
		if (svm_i->check_question_set(gsets[QUESTION]) != 0) {
#ifdef __PRT
			std::cout << std::endl << "check on question set return error." << std::endl;
#endif
			return -1;
		}
#ifdef __PRT
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
		if (svm_i->get_converged(previous_equations, pre_equation_num) == 0) {
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


		pre_equation_num = svm_i->equ_num;
		delete[]previous_equations;
		previous_equations = new Equation[pre_equation_num];
		for (int i = 0; i < svm_i->equ_num; i++) {
			previous_equations[i] = svm_i->equations[i];
		}

	} // end of SVM_I training procedure


	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm_i for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((b_converged) && (rnd <= max_iteration)) {
		int equation_num = -1;
		Equation* equs = svm_i->roundoff(equation_num);
		set_console_color(std::cout, YELLOW);
		std::cout << "  Hypothesis Invairant(Converged): {";
		std::cout << " \n\t ------------------------------------------------------";
		std::cout << " \n\t |     " << equs[0];
		for (int i = 1; i < equation_num; i++) {
			std::cout << " \n\t |  /\\ " << equs[i];
		}
		std::cout << " \n\t ------------------------------------------------------\n";
		unset_console_color(std::cout);
		delete[]equs;
	}

	if ((pass_rate < 1) || (rnd >= max_iteration)) {
		set_console_color(std::cout, RED);
		std::cout << "  Cannot divide by SVM_I perfectly.\n";
		unset_console_color(std::cout);
		ret = -1;
	}

	delete []previous_equations;
	delete svm_i;

	return ret;
}
