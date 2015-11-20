#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>



int minv = -100, maxv = 100;
void print_null(const char *s) {}

States global_states_sets[4];
States* gsets = &global_states_sets[1];


void run_target(Solution inp)
{
	//	std::cout << "inputs" << sol <<  std::endl;
	before_loop();
	//target_program
	m_double(inp.x);
	after_loop();
	if (gsets[COUNT_EXAMPLE].traces_num() > 0) {
		set_console_color(std::cout, RED);
		std::cout << "}\nProgram BUG! Program have encountered a Counter-Example trace." << std::endl;
		std::cout << gsets[COUNT_EXAMPLE] << std::endl;
		unset_console_color(std::cout);
		exit(-1);
	}
}

void init_gsets()
{
	gsets[NEGATIVE].label = -1;
	gsets[QUESTION].label = 0;
	gsets[POSITIVE].label = 1;
	gsets[COUNT_EXAMPLE].label = 2;
}


int main(int argc, char** argv)
{
	/*if (argc < 1) {
	  std::cout << "Arguments less than 2.\n";
	  exit(-1);
	  }*/	
	if (argc >= 3) {
		minv = atoi(argv[1]);
		maxv = atoi(argv[2]);
	}

	Solution inputs;

	init_gsets();
	srand(time(NULL)); // initialize seed for rand() function


	int rnd;
	bool b_similar_last_time = false;
	bool b_converged = false;
	bool b_svm_i = false;
	Equation* p = NULL;
	int pre_positive_size = 0, pre_negative_size = 0; // , pre_question_size = 0;
	int cur_positive_size = 0, cur_negative_size = 0; // , cur_question_size = 0;
	int pre_question_index = 0;


	//Init SVM
	SVM* svm = new SVM(print_null);
	svm->main_equation = NULL;

	for (rnd = 1; rnd <= max_iter; rnd++) {

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
				Equation::linearSolver(NULL, inputs);
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
			int times = 1;

check_in_svm_step1:
			if (times != 1)
				std::cout << "\t    Re-execute programs...[" << times * (after_exes + random_exes) << "] {Random";
			else 
				std::cout << "\t(1) execute programs...[" << times * (after_exes + random_exes) << "] {Random";
			for (int i = 0; i < times * random_exes; i++) {
				Equation::linearSolver(NULL, inputs);
				std::cout << inputs;
				std::cout << "|";
				run_target(inputs);
			}
			std::cout << "Solve";
			for (int i = 0; i < times * after_exes; i++) {
				Equation::linearSolver(p, inputs);
				std::cout << "|" << inputs;
				run_target(inputs);
			}
			std::cout << "}" << std::endl;


			/*
			// test these new examples against the old classifier
			std::cout << "\t    check against last classifier: ";
			cur_positive_size = gsets[POSITIVE].size();
			cur_negative_size = gsets[NEGATIVE].size(); 
			//cur_question_size = gsets[QUESTION].size();
			//bool pass_check_against_old_classifier = false;
			for (int i = pre_positive_size; i < cur_positive_size; i++) {
			if (svm->predict(gsets[POSITIVE].values[i]) < 0) {
			set_console_color(std::cout, RED);
			std::cout << "[Fail Positive]" << std::endl;
			unset_console_color(std::cout);
			goto svm_step2;
			}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Positive]";
			unset_console_color(std::cout);

			for (int i = pre_negative_size; i < cur_negative_size; i++) {
			if (svm->predict(gsets[NEGATIVE].values[i]) >= 0) {
			set_console_color(std::cout, RED);
			std::cout << "[Fail Negative]" << std::endl;
			unset_console_color(std::cout);
			goto svm_step2;
			}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Negative]";
			unset_console_color(std::cout);

			for (int i = pre_question_index; i < gsets[QUESTION].traces_num(); i++) {
			int pre = -1, cur = 0;
			for (int j = gsets[QUESTION].index[i]; j < gsets[QUESTION].index[i + 1]; j++) {
			cur = svm->predict(gsets[QUESTION].values[i]);
			if ((pre >= 0) && (cur < 0)) {
			set_console_color(std::cout, RED);
			std::cout << "[Fail Question]" << std::endl;
			unset_console_color(std::cout);
			goto svm_step2;
			}
			}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Question]" << std::endl;
			unset_console_color(std::cout);

			pre_question_index = gsets[QUESTION].traces_num();

			if (times == 1) {
			std::cout << "\t    Do Recheck..." << std::endl;
			times = final_exes_times;  // BIG executes times for final check.
			goto check_in_svm_step1;
			}
			else {
			set_console_color(std::cout, GREEN);
			std::cout << "[DONE] pass all the checks" << std::endl;
			unset_console_color(std::cout);
			b_converged = true;
			break;
			}
			*/
		} // end of if-statement: else branch for (rnd >= 2)

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
		double passRat = svm->predict_on_training_set();

		if (passRat == 1) set_console_color(std::cout, GREEN);
		else set_console_color(std::cout, RED);
		std::cout << " [" << passRat * 100 << "%]";
		unset_console_color(std::cout);

		if (passRat < 1) {
			std::cout << " [FAIL] \n The problem is not linear separable.. Trying to solve is by SVM-I algo" << std::endl;
			if (p != NULL) {
				Equation* tmp = svm->main_equation;
				svm->main_equation = p;
				double passRat = svm->predict_on_training_set();
				std::cout << " last divide: " << *p << " accuracy[" << passRat * 100 << "%]\n";
				svm->main_equation = tmp;
			}
			std::cerr << "*******************************USING SVM_I NOW******************************" << std::endl;
			b_svm_i = true;
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
		if (svm->main_equation->is_similar(p) == 0) {
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

		if (p != NULL) {
			delete p;
		}
		p = svm->main_equation; 
	} // end of SVM training procedure



	if ((b_converged) || (rnd >= max_iter)) {
		std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
		std::cout << "finish running svm for " << rnd << " times." << std::endl;
		int equation_num = -1;
		Equation* equs = svm->roundoff(equation_num);
		assert(equation_num == 1);
		set_console_color(std::cout);
		if (b_converged)
			std::cout << "  Hypothesis Invairant(Converged): {\n";
		else 
			std::cout << "  Hypothesis Invairant(Reaching Maximium Iteration): {\n";
		std::cout << "\t\t" << equs[0] << std::endl;
		std::cout << "  }" << std::endl;
		unset_console_color(std::cout);
		delete[]equs;
		delete p;
		//delete svm->main_equation;
		delete svm;
		return 0;
	}

	/*if (p == NULL) { // get out svm in the first round, p has not been set yet
		p = svm->main_equation;
	}*/
	delete svm;






	b_similar_last_time = false;
	int pre_equation_num = 1;
	//start SVM_I training
	assert(b_svm_i == true);
	SVM_I* svm_i = new SVM_I(print_null);
	if (p == NULL) 
		svm_i->main_equation = new Equation();

	int svm_i_start = rnd;
	for (; rnd <= max_iter; rnd++) {
		//	init_svm_i:
		std::cout << "[" << rnd << "]SVM-I---------------------------------------------" << "-------------------------------------------------------------" << std::endl;
		if (rnd != svm_i_start) {
			int times = 1;

check_in_svm_i_step1:
			int exes_each_equation = (after_exes + pre_equation_num - 1) / pre_equation_num;
			if (times != 1)
				std::cout << "\t    Re-execute programs...[" << times * (exes_each_equation * pre_equation_num + random_exes) << "] {Random";
			else
				std::cout << "\t(1) execute programs...[" << times * (exes_each_equation * pre_equation_num + random_exes) << "] {Random";

			for (int i = 0; i < times * random_exes; i++) {
				Equation::linearSolver(NULL, inputs);
				std::cout << inputs << "|";
				run_target(inputs);
			}
			p = NULL; //svm_i->main_equation;
			for (int j = 0; j < times * exes_each_equation; j++) {
				Equation::linearSolver(p, inputs);
				std::cout << "|" << inputs;
				run_target(inputs);
			}
			for (int i = 0; i < svm_i->equ_num; i++) {
				p = &(svm_i->equations[i]);
				for (int j = 0; j < exes_each_equation; j++) {
					Equation::linearSolver(p, inputs);
					std::cout << " | " << inputs;
					run_target(inputs);
				}
			}
			std::cout << "}" << std::endl;

			// test these new examples against the old classifier
			std::cout << "\t    check against last classifier: ";
			cur_positive_size = gsets[POSITIVE].size();
			cur_negative_size = gsets[NEGATIVE].size();
			//cur_question_size = gsets[QUESTION].size();
			//bool pass_check_against_old_classifier = false;
			for (int i = pre_positive_size; i < cur_positive_size; i++) {
				if (svm_i->predict(gsets[POSITIVE].values[i]/*, 1*/) < 0) {
					set_console_color(std::cout, RED);
					std::cout << "[Fail Positive]" << std::endl;
					unset_console_color(std::cout);
					goto svm_i_step2;
				}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Positive]";
			unset_console_color(std::cout);

			for (int i = pre_negative_size; i < cur_negative_size; i++) {
				if (svm_i->predict(gsets[NEGATIVE].values[i]/*, 1*/) >= 0) {
					set_console_color(std::cout, RED);
					std::cout << "[Fail Negative]" << std::endl;
					unset_console_color(std::cout);
					goto svm_i_step2;
				}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Negative]";
			unset_console_color(std::cout);

			for (int i = pre_question_index; i < gsets[QUESTION].traces_num(); i++) {
				int pre = -1, cur = 0;
				for (int j = gsets[QUESTION].index[i]; j < gsets[QUESTION].index[i + 1]; j++) {
					cur = svm_i->predict(gsets[QUESTION].values[i]);
					if ((pre >= 0) && (cur < 0)) {
						set_console_color(std::cout, RED);
						std::cout << "[Fail Question]" << std::endl;
						unset_console_color(std::cout);
						goto svm_i_step2;
					}
				}
			}
			set_console_color(std::cout, GREEN);
			std::cout << "[PASS Question]" << std::endl;
			unset_console_color(std::cout);

			pre_question_index = gsets[QUESTION].traces_num();

			if (times == 1) {
				std::cout << "\t    Do Recheck..." << std::endl;
				times = final_exes_times;  // BIG executes times for final check.
				goto check_in_svm_i_step1;
			}
			else {
				set_console_color(std::cout, GREEN);
				std::cout << "[DONE] pass all the checks" << std::endl;
				unset_console_color(std::cout);
				b_converged = true;
				break;
			}
		}
		else {
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
		std::cout << svm_i->equ_num;
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
		/*std::cout << "\t(6) check convergence:        ";
		  if (pre_equation_num == svm_i->equ_num + 1) {
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
		  */
		pre_equation_num = svm_i->equ_num + 1;

		std::cout << std::endl;
	} // end of SVM-I training procedure





	std::cout << "-------------------------------------------------------" << "-------------------------------------------------------------" << std::endl;
	std::cout << "Finish running svm-I for " << rnd - svm_i_start << " times." << std::endl;
	int equation_num = -1;
	Equation* equs = svm_i->roundoff(equation_num);
	set_console_color(std::cout);
	std::cout << "Hypothesis Invairant: ";
	std::cout << " \n\t ------------------------------------------------------";
	std::cout << " \n\t |     " << equs[0];
	for (int i = 1; i < equation_num; i++) {
		std::cout << " \n\t |  /\\ " << equs[i];
	}
	std::cout << " \n\t ------------------------------------------------------";
	std::cout << "\n";

	unset_console_color(std::cout);


	//delete[]equs;
	//delete svm_i->main_equation;
	delete svm_i;
	return 0;
}

