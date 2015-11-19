#ifndef _LIBSVM_II_H_
#define _LIBSVM_II_H_
#include "svm.h"
#include <iostream>
//#include <stdarg.h>
//#include "float.h"



class SVM_II : public SVM
{
private:
	const int max_equ;

public:
	svm_model* model;
	Equation* equations;
	int equ_num;
	svm_parameter param;
	svm_problem problem1;  // 1
	svm_problem problem2;  // -1

	SVM_II(void(*f) (const char*) = NULL, const Equation& eq = 0, int equ = 8) : SVM(f), max_equ(equ) {
		problem1.l = 0;
		problem2.l = 0;

		main_equation = new Equation(eq);
		equ_num = 0;
		equations = new Equation[max_equ];
		model = NULL;

		/*
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
		*/
	}

	~SVM_II()
	{
		if (model != NULL)
			svm_free_and_destroy_model(&model);
		delete[]equations;
	}

	int predict(double* v, int label = 0)
	{
		if (equ_num <= 0) return -2;
		if (v == NULL) return -2;
		/*
		* We use conjunction of positive as predictor.
		* For example, (A >= 0) /\ (B >= 0) /\ (C >= 0) /\ ...
		* Only when the give input pass all the equationss, it returns 1;
		* Otherwise, -1 will be returned.
		*/
		double res = Equation::calc(main_equation, v);
		if (res < 0) return -1;
		for (int i = 0; i < equ_num; i++) {
			res = Equation::calc(&equations[i], v);
			if (res < 0) return -1;
		}
		return 1;
	}


	double predictOnProblem()
	{
		int total = problem1.l + problem2.l;
		int pass = 0;
		if (problem1.l > 0) {
			for (int i = 0; i < problem1.l; i++) {
				pass += (predict((double*)problem1.x[i]) >= 0) ? 1 : 0;
			}
		}
		if (problem2.l > 0) {
			for (int i = 0; i < problem2.l; i++) {
				pass += (predict((double*)problem2.x[i]) < 0) ? 1 : 0;
			}
		}
		return (double)pass / total;
		return 0;
	}


	double CheckPostive()
	{
		int total = problem1.l;
		int pass = 0;
		std::cout << "\t";
		if (problem1.l > 1) {
			for (int i = 0; i < problem1.l; i++) {
				bool bPass = (Equation::calc(&equations[equ_num], (double*)problem1.x[i]) * problem1.y[i] >= 0) ? 1 : 0;
				pass += bPass;
			}
		}
		std::cout << std::endl << pass << "/" << total << "..";
		return (double)pass / total;
	}


	int getMisclassified(int& idx) // negative points may be misclassified.
	{
		if ((problem2.y == NULL) || (problem2.x == NULL))
			return -1;
		if (equ_num <= 0) {
			idx = 0;
			return 0;
		}
		for (int i = 0; i < problem2.l; i++)
			if (predict((double*)problem2.x[i]) >= 0) {
				idx = i;
				return 0;
			}
		idx = -1;
		return 0;
	}


	int train()
	{
		if (problem1.y == NULL || problem1.x == NULL
			|| problem2.y == NULL || problem2.x == NULL)
			return -1;


		for (equ_num = 0; equ_num < max_equ; equ_num++) {
			int misidx = -1;
			int ret = getMisclassified(misidx);
			if (ret == -1) return -1;
			if ((ret == 0) && (misidx == -1)) {
				std::cout << "finish classified..." << std::endl;
				return 0;
			}

			int length = problem1.l;
			problem1.y[length] = -1;
			problem1.x[length] = problem2.x[misidx];
			problem1.l++;

			std::cout << "." << equ_num; // << std::endl;

			model = svm_train(&problem1, &param);
			svm_model_visualization(model, &equations[equ_num]);

			double precision = CheckPostive();
			svm_free_and_destroy_model(&model);
			problem1.l--;
			std::cout << " On training set precision: " << precision * 100 << "%" << std::endl;
			//std::cout << "\n On whole set precision: " << predictOnProblem() * 100 << "%" << std::endl;
		}
		return 0;
	}


	friend std::ostream& operator << (std::ostream& out, const SVM_II sii) {
		out << "Learnt from SVM-II...";
		if (sii.equ_num <= 0) {
			out << "Having Learnt...\n";
			return out;
		}
		out << std::setprecision(16);
		out << "{ \n\t    " << *sii.main_equation;
		for (int i = 0; i < sii.equ_num; i++) {
			out << " \n\t /\\ " << sii.equations[i];
		}
		out << "}\n";
		return out;
	}


	int size()
	{
		return problem1.l + problem2.l;
	}


};


#endif /* _LIBSVM_II_H */
