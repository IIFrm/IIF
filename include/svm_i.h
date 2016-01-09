#ifndef _SVM_I_H_
#define _SVM_I_H_

#include "svm.h"
#include "color.h"
#include <iostream>


class SVM_I : public MLalgo //SVM
{
	protected:
		svm_parameter param;
		svm_problem problem;
		svm_model* model;
		Equation* classifiers;
		int csf_num;

		int max_size;
		int max_csf;

		double* label; // [max_items * 2];
		double** data; // [max_items * 2];

		States* negatives; 

	protected:
		int enlargeSize(int new_size) {
			assert (new_size >= max_size);
			int valid_size = problem.l;

			// enlarge max_size exponentially to cover all the data.
			while (new_size >= max_size) max_size *= 2;

			double** pre_data = data;
			data = new double*[max_size];
			memmove(data, pre_data, valid_size * sizeof(double**));
			delete []pre_data;

			double* pre_label = label;
			label = new double[max_size];
			memmove(label, pre_label, valid_size * sizeof(double*));
			delete pre_label;
			return 0;
		}

	public:
		inline int getClassifierNum() {
			return csf_num;
		}

		inline Equation* getClassifier() {
			return classifiers;
		}

		inline Equation* getClassifier(int idx) {
			if (idx >= csf_num) return NULL;
			return &classifiers[idx];
		}

		inline Equation* saveClassifier() {
			Equation* es = new Equation[csf_num];
			for (int i = 0; i < csf_num; i++)
				es[i] = classifiers[i];
			return es;
		}

		SVM_I(void(*f) (const char*) = NULL, int size = 10000, int csf = 16) : max_size(size), max_csf(csf) {
			prepare_linear_parameters(param);
			if (f != NULL)
				svm_set_print_string_function(f);
			model = NULL;
			classifiers = new Equation[max_csf];

			data = new double*[max_size];
			label = new double[max_size];
			for (int i = 0; i < max_size; i++)
				label[i] = -1;
			problem.l = 0;
			problem.x = (svm_node**)(data);
			problem.y = label;

			negatives = NULL;
			csf_num = 0;
		}

		~SVM_I()
		{
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (classifiers != NULL) delete []classifiers;
			if (data != NULL) delete []data;
			if (label != NULL) delete []label;
		}

		int makeTrainingSet(States* gsets, int& pre_psize, int& pre_nsize) {
			negatives = &gsets[NEGATIVE];
			int cur_psize = gsets[POSITIVE].getSize();
			int cur_nsize = gsets[NEGATIVE].getSize();

			if (cur_psize + 1 >= max_size)
				enlargeSize(cur_psize + 1);

#ifdef __PRT
			std::cout << "+[" << cur_psize - pre_psize << "|"
				<< cur_nsize - pre_nsize  << "] ==> ["
				<< cur_psize << "+|" << cur_nsize << "-]";
#endif

			// prepare new training data set
			// training set & label layout:
			// data :  0 | positive states ...
			// add new positive states at OFFSET: [pre_psize]
			for (int i = 0/*pre_psize*/; i < cur_psize; i++) {
				data[i] = gsets[POSITIVE].values[i];
				label[i] = 1;
			}

			problem.l = cur_psize;
			pre_psize = cur_psize;
			pre_nsize = cur_nsize;
			return 0;
		}

		int train()
		{
			if (problem.y == NULL || problem.x == NULL || negatives == NULL) return -1;

			for (csf_num = 0; csf_num < max_csf; csf_num++) {
				int misidx = -1;
				int ret = getMisclassified(misidx);
				if (ret == -1) return -1;  // something wrong in misclassified.
				if ((ret == 0) && (misidx == -1)) {	// can divide all the negative points correctly

#ifdef __PRT
					std::cout << GREEN << "finish classified..." << WHITE << std::endl;
#endif				
#ifdef __PRT_SVM_I
					std::cout << *this << "\n check implication...\n";
#endif				

					/*
					   for (int i = 1; i < equ_num; i++) {
					   for (int j = 0; j < i; j++) {
					   if (classifiers[i].imply(classifiers[j]) == true) {
					   for (int k = j; k < equ_num - 1; k++)
					   classifiers[k] = classifiers[k+1];
					   equ_num--; i--; j--;
					   }
					   }
					   }
					   setColor(std::cout, RED);
					//std::cout << *this << "\n";
					setColor(std::cout);
					*/

					return 0;
				}

#ifdef __PRT_SVM_I
				std::cout << "." << csf_num << ">"; // << std::endl;
#endif
				// there is some point which is misclassified by current dividers.
				// "negatives->values[misidx]" 

				int length = problem.l;
				label[length] = -1;
				data[length] = negatives->values[misidx];
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
				svm_model_visualization(model, classifiers[csf_num]);

#ifdef __PRT_SVM_I
				std::cout << classifiers[csf_num];
#endif
				for (int j = 0; j < csf_num; j++) {
					if (classifiers[csf_num].imply(classifiers[j]) == true) {
						for (int k = j; k < csf_num; k++)
							classifiers[k] = classifiers[k+1];
						csf_num--;
						j--;
					}
				}


#ifdef __PRT_SVM_I
				double precision = checkStepTrainingData();
#endif
				svm_free_and_destroy_model(&model);
				model = NULL;
				problem.l--;

				//if (precision == 1) break;
#ifdef __PRT_SVM_I
				std::cout << " precision=[" << precision * 100 << "%]." << std::endl;
				//if (precision < 1) std::cout << "CAN NOT DIVIDE THE PICKED NEGATIVE FROM POSITIVES..." << std::endl;
				//std::cout << "\n On whole set precision: " << predictOnProblem() * 100 << "%" << std::endl;
#endif
			}
			std::cout << RED << "Can not divide all the data by SVM-I with"
				" equations number limit to " << csf_num + 1 << "." << std::endl;
			std::cerr << "You need to increase the limit by modifying [classname::methodname]"
				"=SVM-I::SVM-I(..., int equ = **) " << WHITE << std::endl;
			return -1;
		}

		double checkTrainingSet()
		{
			int nsize = negatives->getSize();
			int total = problem.l + nsize;
			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (predict((double*)problem.x[i]) >= 0) ? 1 : 0;
			}
			if (nsize > 0) {
				for (int i = 0; i < nsize; i++) {
					pass += (predict(negatives->values[i]) < 0) ? 1 : 0;
				}
			}
			return (double)pass / total;
		}

		virtual int checkQuestionSet(States& qset)
		{
			return 0;
#ifdef __PRT
			std::cout << " [" << qset.traces_num() << "]";
#endif 
			for (int i = 0; i < qset.p_index; i++) {
				int pre = -1, cur = 0;
#ifdef __PRT
				std::cout << GREEN << ".";
#endif 

				for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
					cur = predict(qset.values[j]);
					//std::cout << ((cur >= 0) ? "+" : "-");
					if ((pre > 0) && (cur < 0)) {
						// deal with wrong question trace.
						// Trace back to print out the whole trace and the predicted labels.
#ifdef __PRT
						std::cerr << RED << "\t[FAIL]\n \t  Predict wrongly on Question trace";
						qset.dumpTrace(i);
#endif 
						for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
							cur = predict(qset.values[j]);
#ifdef __PRT
							std::cout << ((cur >= 0) ? "+" : "-");
#endif 
						}
#ifdef __PRT
						std::cout << WHITE << std::endl;
#endif 
						return -1;
					}
					pre = cur;
				}
			}
#ifdef __PRT
			std::cout << " [PASS]" << WHITE;
#endif 
			return 0;
		}

		int converged(void* pre_model, int pre_csf_num) {
			if (pre_csf_num != csf_num) return -1;
			
			Equation* pre_classifiers = (Equation*)pre_model;
			std::vector<bool> similar_vector(pre_csf_num, false);
			for (int i = 0; i < pre_csf_num; i++) {	
				// for all the equations in current state
				for (int j = 0; j < pre_csf_num; j++) {	
					// check all the equations in last state
					if ((similar_vector[j] == false) && (classifiers[i].is_similar(pre_classifiers[j]) == 0))  {	
						// the equation in last has not been set
						// and it is similar to the current equation 
#ifdef __PRT
						std::cout << GREEN <<  "<" << i << "-" << j << "> " << WHITE;
#endif 
						similar_vector[j] = true;
						break;
					} 
#ifdef __PRT
					else {
						std::cout << RED << "<" << i << "-" << j << "> " << WHITE;
					}
#endif 
				}
			}
			for (int i = 0; i < pre_csf_num; i++) {
				if (similar_vector[i] == false) {
					similar_vector.clear();
					return -1;
				}
			}
			similar_vector.clear();
			return 0;
		}

		friend std::ostream& operator << (std::ostream& out, const SVM_I& svm_i) {
			return svm_i._print(out);
		}

		std::ostream& _print(std::ostream& out) const {
			out << "SVM-I: ";
			out << std::setprecision(16);
			out << " \n\t ------------------------------------------------------";
			out << " \n\t |     " << classifiers[0];
			for (int i = 1; i < csf_num; i++) {
				out << " \n\t |  /\\ " << classifiers[i];
			}
			out << " \n\t ------------------------------------------------------";
			return out;
		}

		int getProblemSize() {
			if (negatives == NULL) return problem.l;
			return problem.l + negatives->getSize();
		}

		Equation* roundoff(int& num) {
			num = csf_num;
			Equation* equs = new Equation[csf_num];
			for (int i = 0; i < csf_num; i++)
				classifiers[i].roundoff(equs[i]);
			return equs;
		}

		int predict(double* v)
		{
			if (v == NULL) return -2;
			/*
			 * We use conjunction of positive as predictor.
			 * For example, (A >= 0) /\ (B >= 0) /\ (C >= 0) /\ ...
			 * Only when the give input pass all the equationss, it returns 1;
			 * Otherwise, -1 will be returned.
			 */
			double res = 0;
			if (res >= 0) {
				for (int i = 0; i < csf_num; i++) {
					res = Equation::calc(classifiers[i], v);
					if (res < 0) break;
				}
			}

			return (res >= 0) ? 1 : -1;
		}

	private:
		//double check_postives_and_one_negative()
		double checkStepTrainingData()
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

		int getMisclassified(int& idx) // negative points may be misclassified.
		{
			if (negatives == NULL) return -1;
			if ((csf_num < 0)) { 
				idx = 0;
				return -1;
			} else if (csf_num == 0) {
				idx = 0;
				return 0;
			}

			int negatives_size = negatives->getSize();
			for (int i = 0; i < negatives_size; i++) {
				if (predict(negatives->values[i]) >= 0) {
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
};

#endif /* _SVM_I_H */
