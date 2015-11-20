#include "svm_i.h"
#include "string.h"
#include <vector>

SVM_I::SVM_I(void(*f) (const char*), int size, int equ) : SVM(f, size), max_equ(equ) {
	negatives = NULL;
	//main_equation = eq;
	equ_num = 0;
	equations = new Equation[max_equ];
	model = NULL;


	param.svm_type = C_SVC;
	param.kernel_type = LINEAR;
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	//	param.C = 1;
	//param.C = DBL_MAX;
	param.C = 1000;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	if (f != NULL)
		svm_set_print_string_function(f);


	//for (int i = 0; i < max_size; i++)
	//	training_label[i] = -1;
	problem.x = (svm_node**)(training_set);
	problem.y = training_label;

	//param.C = DBL_MAX;
}

SVM_I::~SVM_I()
{
	if (model != NULL)
		svm_free_and_destroy_model(&model);
	delete []equations;
}


int SVM_I::prepare_training_data(States* gsets, int& pre_positive_size, int& pre_negative_size) {
	negatives = &gsets[NEGATIVE];
	int cur_positive_size = gsets[POSITIVE].size();
	int cur_negative_size = gsets[NEGATIVE].size();

	if (cur_positive_size + 1 >= max_size) {
		int previous_max_size = max_size;
		while (cur_positive_size + 1 >= max_size)
			max_size *= 2;
		double** previous_training_set = training_set;
		training_set = new double*[max_size];
		memmove(training_set, previous_training_set, previous_max_size * sizeof(double**));

		double* previous_training_label = training_label;
		training_label = new double[max_size];
		memmove(training_label, previous_training_label, previous_max_size * sizeof(double*));
	}

	std::cout << "+[";
	std::cout << cur_positive_size - pre_positive_size << "|";
	std::cout << cur_negative_size - pre_negative_size << "";
	std::cout << "] ==> [";
	std::cout << cur_positive_size << "+|";
	std::cout << cur_negative_size << "-";
	std::cout << "]";


	// prepare new training data set
	// training set & label layout:
	// data :  0 | positive states ...
	// add new positive states at OFFSET: [pre_positive_size]
	for (int i = 0/*pre_positive_size*/; i < cur_positive_size; i++) {
		training_set[i] = gsets[POSITIVE].values[i];
		training_label[i] = 1;
	}

	problem.l = cur_positive_size;
	pre_positive_size = cur_positive_size;
	pre_negative_size = cur_negative_size;
	return 0;
}


int SVM_I::train()
{
	if (problem.y == NULL || problem.x == NULL || negatives == NULL) return -1;

	for (equ_num = 0; equ_num < max_equ; equ_num++) {
		int misidx = -1;
		int ret = get_misclassified(misidx);
		if (ret == -1) return -1;  // something wrong in misclassified.
		if ((ret == 0) && (misidx == -1)) {	// can divide all the negative points correctly

#ifdef __PRT
			set_console_color(std::cout, GREEN);
			std::cout << "finish classified..." << std::endl;
			unset_console_color(std::cout);
#endif				
#ifdef __PRT_SVM_I
			std::cout << *this << "\n check implication...\n";
#endif				

			/*
			for (int i = 1; i < equ_num; i++) {
				for (int j = 0; j < i; j++) {
					if (equations[i].imply(equations[j]) == true) {
						for (int k = j; k < equ_num - 1; k++)
							equations[k] = equations[k+1];
						equ_num--; i--; j--;
					}
				}
			}
			set_console_color(std::cout, RED);
			//std::cout << *this << "\n";
			unset_console_color(std::cout);
			*/

			return 0;
		}

#ifdef __PRT_SVM_I
		std::cout << "." << equ_num << ">"; // << std::endl;
#endif
		// there is some point which is misclassified by current dividers.
		// "negatives->values[misidx]" 

		int length = problem.l;
		training_label[length] = -1;
		training_set[length] = negatives->values[misidx];
		problem.l++;

#ifdef __PRT_SVM_I
		std::cout << " NEW TRAINING SET:";
		for (int i = 0; i < problem.l; i++) {
			std::cout << "(" << problem.x[i][0].value;
			for (int j = 1; j < VARS; j++)
				std::cout << "," << problem.x[i][j].value;
			std::cout << ")";
			if (problem.y[i] == 1) std::cout << "+";
			if (problem.y[i] == -1) std::cout << "-";
			std::cout << "|";
		}
		std::cout << std::endl;
#endif			

		model = svm_train(&problem, &param);
		svm_model_visualization(model, &equations[equ_num]);

#ifdef __PRT_SVM_I
		std::cout << equations[equ_num];
#endif
		for (int j = 0; j < equ_num; j++) {
			if (equations[equ_num].imply(equations[j]) == true) {
				for (int k = j; k < equ_num; k++)
					equations[k] = equations[k+1];
				equ_num--; j--;
			}
		}


#ifdef __PRT_SVM_I
		double precision = check_postives_and_one_negative();
#endif
		svm_free_and_destroy_model(&model);
		problem.l--;

		//if (precision == 1) break;
#ifdef __PRT_SVM_I
		std::cout << " precision=[" << precision * 100 << "%]." << std::endl;
		//if (precision < 1) std::cout << "CAN NOT DIVIDE THE PICKED NEGATIVE FROM POSITIVES..." << std::endl;
		//std::cout << "\n On whole set precision: " << predictOnProblem() * 100 << "%" << std::endl;
#endif
	}
	set_console_color(std::cout, RED);
	std::cout << "Can not divide all the data by SVM-I with equations number limit to " << equ_num + 1 << "." << std::endl;
	std::cerr << "You can increase the limit by modifying [classname::methodname]=SVM-I::SVM-I(..., int equ = **) " << std::endl;
	unset_console_color(std::cout);
	return -1;
}

double SVM_I::predict_on_training_set()
{
	int negatives_size = negatives->size();
	int total = problem.l + negatives_size;
	int pass = 0;
	for (int i = 0; i < problem.l; i++) {
		pass += (predict((double*)problem.x[i]) >= 0) ? 1 : 0;
	}
	if (negatives_size > 0) {
		for (int i = 0; i < negatives_size; i++) {
			pass += (predict(negatives->values[i]) < 0) ? 1 : 0;
		}
	}
	return (double)pass / total;
}

int SVM_I::check_question_set(States& qset) {
	//if (main_equation == NULL) return -1;
	std::cout << " [" << qset.traces_num() << "]";
	for (int i = 0; i < qset.p_index; i++) {
		int pre = -1, cur = 0;
		set_console_color(std::cout, GREEN);
		std::cout << ".";
		//std::cout << "\t\t" << i << ">";
		//gsets[QUESTION].print_trace(i);

		for (int j = qset.index[i]; j < qset.index[i + 1]; j++) {
			cur = predict(qset.values[j]);
			//std::cout << ((cur >= 0) ? "+" : "-");
			if ((pre > 0) && (cur < 0)) {
				// deal with wrong question trace.
				// Trace back to print out the whole trace and the predicted labels.
				set_console_color(std::cout, RED);
				std::cerr << "\t\t[FAIL]\n \t  Predict wrongly on Question trace";
				qset.print_trace(i);
				for (int j = qset.index[i]; j < qset.index[i + 1]; j++) {
					cur = predict(qset.values[j]);
					std::cout << ((cur >= 0) ? "+" : "-");
				}
				std::cout << std::endl;
				unset_console_color(std::cout);
				return -1;
			}
			pre = cur;
		}
		//std::cout << "END" << std::endl;
	}
	std::cout << " [PASS]";
	unset_console_color(std::cout);
	return 0;
}


int SVM_I::get_converged(Equation* previous_equations, int previous_equation_num) {
	//std::cout << "\n\tPrevious\t\t\t Current\n";
	//std::cout << "\t" << previous_equation_num << "\t\t\t\t\t" << equ_num << "\n";

	if (previous_equation_num != equ_num) return -1;
	/*for (int i = 0; i < previous_equation_num; i++) {	// for all the equations in current state
		std::cout << previous_equations[i] << "\t\t" << equations[i] << "\n";
	} */


	std::vector<bool> similar_vector(previous_equation_num, false);
	for (int i = 0; i < previous_equation_num; i++) {	// for all the equations in current state
		for (int j = 0; j < previous_equation_num; j++) {	// check all the equations in last state
			if ((similar_vector[j] == false) && (equations[i].is_similar(previous_equations[j]) == 0))  {	
				// the equation in last has not been set
				// and it is similar to the current equation 
				set_console_color(std::cout, GREEN);
				std::cout << "<" << i << "-" << j << "> ";
				similar_vector[j] = true;
				unset_console_color(std::cout);
				break;
			} else {
				set_console_color(std::cout, RED);
				std::cout << "<" << i << "-" << j << "> ";
				unset_console_color(std::cout);
			}
		}
	}
	for (int i = 0; i < previous_equation_num; i++) {
		if (similar_vector[i] == false) {
			similar_vector.clear();
			return -1;
		}
	}
	similar_vector.clear();
	return 0;
}


std::ostream& operator << (std::ostream& out, const SVM_I& svm_i) {
	return svm_i._print(out);
}

std::ostream& SVM_I::_print(std::ostream& out) const {
	out << "SVM-I: ";
	out << std::setprecision(16);
	out << " \n\t ------------------------------------------------------";
	out << " \n\t |     " << equations[0];
	for (int i = 1; i < equ_num; i++) {
		out << " \n\t |  /\\ " << equations[i];
	}
	out << " \n\t ------------------------------------------------------";
	return out;
}


int SVM_I::size()
{
	if (negatives == NULL)
		return problem.l;
	return problem.l + negatives->size();
}


Equation* SVM_I::roundoff(int& num)
{
	num = equ_num; // + 1
	Equation* equs = new Equation[equ_num]; // + 1];
	//main_equation->roundoff(equs[0]);
	for (int i = 0; i < equ_num; i++)
		equations[i].roundoff(equs[i]);
	return equs;
}


int SVM_I::predict(double* v, int label)
{
	if (v == NULL) return -2;
	/*
	 * We use conjunction of positive as predictor.
	 * For example, (A >= 0) /\ (B >= 0) /\ (C >= 0) /\ ...
	 * Only when the give input pass all the equationss, it returns 1;
	 * Otherwise, -1 will be returned.
	 */
	//double res = Equation::calc(main_equation, v);
	double res = 0;
	if (res >= 0) {
		for (int i = 0; i < equ_num; i++) {
			res = Equation::calc(equations[i], v);
			if (res < 0)
				break;
		}
	}

#ifdef __PRT_SVM_I
	if (label == 0) {
		return (res >= 0) ? 1 : -1;
	}
	if (res * label >= 0)
		set_console_color(std::cout, GREEN);
	else 
		set_console_color(std::cout, RED);

	std::cout << "(" << v[0];
	for (int i = 1; i < VARS; i++)
		std::cout << "," << v[i];
	std::cout << ")";
	unset_console_color(std::cout);
#endif
	return (res >= 0) ? 1 : -1;
}

double SVM_I::check_postives_and_one_negative()
{
	int total = problem.l;
	int pass = 0;
#ifdef __PRT_SVM_I
	std::cout << "\t";
#endif
	for (int i = 0; i < problem.l; i++) {
		pass += (predict((double*)(problem.x[i])) * problem.y[i] >= 0) ? 1 : 0;
	}
#ifdef __PRT_SVM_I
	std::cout << std::endl << "Check on training set result: " << pass << "/" << total << "..";
#endif
	return (double)pass / total;
}


int SVM_I::get_misclassified(int& idx) // negative points may be misclassified.
{
	if (negatives == NULL)
		return -1;
	if ((equ_num < 0)) { // && (main_equation == NULL)) {
		idx = 0;
		return -1;
	} else if (equ_num == 0) {
		idx = 0;
		return 0;
	}

	int negatives_size = negatives->size();
	for (int i = 0; i < negatives_size; i++) {
		if (predict(negatives->values[i], -1) >= 0) {
#ifdef __PRT_SVM_I
			std::cout << "\n [FAIL] @" << i << ": (" << negatives->values[i][0];
			for (int j = 1; j < VARS; j++)
				std::cout << "," << negatives->values[i][j];
			std::cout << ")  \t add it to training set... ==>" << std::endl;
#endif
			idx = i;
			return 0;
		}
	}

	// there should be no negatives misclassified.
#ifdef __PRT_SVM_I
	std::cout << "\n [PASS] @all";
#endif
	idx = -1;
	return 0;	
	}
