#ifndef _SVM_I_H_
#define _SVM_I_H_

#include "svm.h"
#include "color.h"
#include <iostream>


class SVM_I : public MLalgo //SVM
{
	protected:
		svm_parameter param;
		svm_model* model;
		int poly_num;

		int max_size;
		int max_poly;

		MState* raw_mapped_data;
		double** data; // [max_items * 2];
		double* label; // [max_items * 2];


		int resize(int new_size) {
			if (new_size <= max_size) return 0;
			assert (new_size >= max_size);
			int valid_size = problem.l;

			// enlarge max_size exponentially to cover all the data.
			while (new_size >= max_size) max_size *= 2;

			double** new_data = new double*[max_size];
			memmove(new_data, data, valid_size * sizeof(double**));
			delete []data;
			data = new_data;

			double* new_label = new double[max_size];
			memmove(new_label, label, valid_size * sizeof(double*));
			delete label;
			label = new_label;
			return 0;
		}

	public:
		svm_problem problem;
		MState* negative_mapped_data; 
		int negative_size;

		SVM_I(int type = 0, void(*f) (const char*) = NULL, int size = 10000, int n_poly = 16) 
			: max_size(size), max_poly(n_poly) {
				prepare_svm_parameters(param, type);
				if (f != NULL)
					svm_set_print_string_function(f);
				model = NULL;
				//polys = new Polynomial[max_poly];

				raw_mapped_data = new MState[max_size];
				data = new double*[max_size];
				label = new double[max_size];
				for (int i = 0; i < max_size; i++)
					label[i] = -1;
				problem.l = 0;
				problem.x = (svm_node**)(data);
				problem.y = label;

#ifdef __DS_ENABLED
				problem.np = 0;
				problem.nn = 0;
#endif

				etimes = 0;
				poly_num = 0;
				negative_mapped_data = new MState[max_size];
				negative_size = 0;
			}

		~SVM_I() {
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (raw_mapped_data != NULL) delete []raw_mapped_data;
			if (data != NULL) delete []data;
			if (label != NULL) delete []label;
		}

		int makeTrainingSet(States* gsets, int& pre_psize, int& pre_nsize) {
			int cur_psize = gsets[POSITIVE].getSize();
			int cur_nsize = gsets[NEGATIVE].getSize();

			if (cur_psize + 1 >= max_size)
				resize(cur_psize + 1);

#ifdef __PRT
			std::cout << "++[" << cur_psize - pre_psize << "|"
				<< cur_nsize - pre_nsize  << "] ==> ["
				<< cur_psize << "+|" << cur_nsize << "-]";
#endif

			// prepare new training data set
			// training set & label layout:
			// data :  0 | positive states ...
			// add new positive states at OFFSET: [pre_psize]
			int cur_index = pre_psize;
			for (int i = 0; i < cur_psize - pre_psize; i++) {
				mappingData(gsets[POSITIVE].values[pre_psize + i], raw_mapped_data[cur_index + i], 4);
				data[cur_index + i] = raw_mapped_data[cur_index + i];
				label[cur_index + i] = 1;
			}
			cur_index = pre_nsize;
			for (int i = 0; i < cur_nsize - pre_nsize; i++) {
				mappingData(gsets[NEGATIVE].values[pre_nsize + i], negative_mapped_data[cur_index + i], 4);
			}
			negative_size = cur_nsize;

#ifdef __DS_ENABLED
			problem.np = cur_psize;
			problem.nn = cur_nsize;
#endif

			problem.l = cur_psize;
			int ret = cur_psize + cur_nsize - pre_psize - pre_nsize;
			pre_psize = cur_psize;
			pre_nsize = cur_nsize;
			return ret;
		}


		int train() {
			if (problem.y == NULL || problem.x == NULL || negative_mapped_data == NULL) return -1;

			for (cl.size = 0; cl.size < cl.max_size;) {
				int misidx = -1;
				int ret = getMisclassified(misidx);
				if (ret == -1) return -1;  // something wrong in misclassified.
				if ((ret == 0) && (misidx == -1)) {	// can divide all the negative points correctly

#ifdef __PRT
					std::cout << GREEN << "finish classified..." << NORMAL << std::endl;
#endif				
					return 0;
				}

#ifdef __PRT_SVM_I
				std::cout << "." << cl.size << ">"; // << std::endl;
#endif
				// there is some point which is misclassified by current dividers.
				if (stepTrain(misidx) < 0) {
					std::cout << "step train (" << misidx << ") return < 0, " << problem.l + negative_size << "\n";
					return -1;
				}
			}
			std::cout << RED << "Can not divide all the data by SVM-I with"
				" equations number limit to " << cl.size + 1 << "." << std::endl;
			std::cerr << "You need to increase the limit by modifying [classname::methodname]"
				"=SVM-I::SVM-I(..., int equ = **) " << NORMAL << std::endl;
			return -1;
		}

		double checkTrainingSet()
		{
			int total = problem.l + negative_size;
			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				/*
				   std::cout << problem.x[i][0];
				   for (int j = 1; j < Nv; j++)
				   std::cout << "," << problem.x[i][j];
				   std::cout << GREEN << "-1" << "->" << predict((double*)problem.x[i]) << NORMAL << " ";
				   */
				pass += (predict((double*)problem.x[i]) >= 0) ? 1 : 0;
			}
			for (int i = 0; i < negative_size; i++) {
				/*
				   std::cout << negative_mapped_data[i][0];
				   for (int j = 1; j < Nv; j++)
				   std::cout << "," << negative_mapped_data[i][j];
				   std::cout << GREEN << "-1" << "->" << predict(negative_mapped_data[i]) << NORMAL << " ";
				   */
				pass += (predict(negative_mapped_data[i]) < 0) ? 1 : 0;
			}
			//std::cout << "<total=" << total << " pass=" << pass << ">" << std::endl;
			return (double)pass / total;
		}

		bool converged(Classifier& pre_cl) {
			if (pre_cl.size != cl.size) return false;

			std::vector<bool> similar_vector(cl.size, false);
			for (int i = 0; i < pre_cl.size; i++) {	
				// for all the equations in current state
				for (int j = 0; j < pre_cl.size; j++) {	
					// check all the equations in last state
					if ((similar_vector[j] == false) && (cl[i]->isSimilar(*pre_cl[j]) == true))  {	
						// the equation in last has not been set
						// and it is similar to the current equation 
#ifdef __PRT
						std::cout << GREEN <<  "<" << i << "-" << j << "> " << NORMAL;
#endif 
						similar_vector[j] = true;
						break;
					} 
#ifdef __PRT
					else {
						std::cout << RED << "<" << i << "-" << j << "> " << NORMAL;
					}
#endif 
				}
			}
			for (int i = 0; i < pre_cl.size; i++) {
				if (similar_vector[i] == false) {
					similar_vector.clear();
					return false;
				}
			}
			similar_vector.clear();
			return true;
		}


		friend std::ostream& operator << (std::ostream& out, const SVM_I& svm_i) {
			return svm_i._print(out);
		}

		std::ostream& _print(std::ostream& out) const {
			out << cl;
			return out;
			//out << std::setprecision(16);
			out << " \n\t ------------------------------------------------------";
			out << " \n\t |     " << cl[0]->toString();
			for (int i = 1; i < cl.size; i++) {
				out << " \n\t |  /\\ " << cl[i]->toString();
			}
			out << " \n\t ------------------------------------------------------";
			return out;
		}

		int getProblemSize() {
			return problem.l + negative_size;
		}


		int predict(double* v) {
			if (v == NULL) return -2;
			/*
			 * We use conjunction of positive as predictor.
			 * For example, (A >= 0) /\ (B >= 0) /\ (C >= 0) /\ ...
			 * Only when the give input pass all the equationss, it returns 1;
			 * Otherwise, -1 will be returned.
			 */
			double res = 0;
			if (res >= 0) {
				for (int i = 0; i < cl.size; i++) {
					res = Polynomial::calc(*cl[i], v);
					if (res < 0) break;
				}
			}

			return (res >= 0) ? 1 : -1;
		}

	private:
		double checkStepTrainingData() {
			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (predict((double*)(problem.x[i])) * problem.y[i] >= 0) ? 1 : 0;
#ifdef __PRT_SVM_I
				double predict_result = predict((double*)(problem.x[i]));
				if (predict_result * problem.y[i] < 0) {
					std::cout << RED << "Predict fault on: [" << problem.x[i][0];
					for (int j = 1; j < Nv; j++)
						std::cout << "," << problem.x[i][j];
					std::cout << "]" << predict_result << ":" << problem.y[i] << NORMAL;
				}
#endif
			}
#ifdef __PRT_SVM_I
			std::cout << "\t";
			std::cout << "\nCheck on training set result: " << pass << "/" << problem.l << "..";
#endif
			return (double)pass / problem.l;
		}

		int stepTrain(int negative_index) {
			if ((negative_index < 0) || (negative_index >= negative_size))
				return -1;
			label[problem.l] = -1;
			data[problem.l] = negative_mapped_data[negative_index];
			problem.l++;

#ifdef __PRT_SVM_I
			std::cout << " NEW TRAINING SET:";
			for (int i = 0; i < problem.l; i++) {
				std::cout << "(" << problem.x[i][0].value;
				for (int j = 1; j < Nv; j++)
					std::cout << "," << problem.x[i][j].value;
				std::cout << ")";
				if (problem.y[i] == 1) std::cout << "+";
				if (problem.y[i] == -1) std::cout << "-";
				std::cout << "|";
			}
			std::cout << std::endl;
#endif			

			double precision = 0;
			int et;
			for (et = 1; et <= 4; et++) {
				setEtimes(et);
				model = svm_train(&problem, &param);
				Polynomial poly;
				svm_model_visualization(model, &poly);
				cl += poly;
				precision = checkStepTrainingData();
				svm_free_and_destroy_model(&model);

#ifdef __PRT_SVM_I
				std::cout << GREEN <<  poly << "\n" << NORMAL;
				//std::cout << cl;
				std::cout << " precision=[" << precision * 100 << "%]." << std::endl;
				if (precision < 1) {
					std::cout << "[" << problem.x[problem.l - 1][0].value;
					for (int j = 1; j < Nv; j++)
						std::cout << "," << problem.x[problem.l - 1][j].value;
					std::cout << "] " << problem.y[problem.l - 1] << " --> ";
					std::cout << poly << " ";
					std::cout << " --> precision=[" << precision * 100 << "%]." << std::endl;
				}
#endif
				//if (precision < 1) std::cout << "CAN NOT DIVIDE THE PICKED NEGATIVE FROM POSITIVES...\n";
				//std::cout << "\n On whole set precision: " << predictOnProblem() * 100 << "%\n";
				//std::cout << " <et=" << et << ",Precision=" << precision << ",cl.size=" << cl.size << ">" << cl;
				if (precision == 1)
					break;
				cl.size--;
			}
			//std::cin.get();
			problem.l--;
			if (et > 4) {
				std::cout << "et = " << et << "\n";
				return -1;
			}
			cl.resolveUniImplication();
			//std::cout << " ---> < cl.size=" << cl.size << ">" << cl << std::endl;
			model = NULL;
			//std::cout << negative_index << BLUE << ">>" <<cl.toString() << std::endl << NORMAL;
			return 0;
		}

		// negative points may be misclassified.
		int getMisclassified(int& idx) {
			idx = 0;
			if (cl.size < 0) return -1;
			if (cl.size == 0) return 0;

			for (int i = 0; i < negative_size; i++) {
				if (predict(negative_mapped_data[i]) >= 0) {
#ifdef __PRT_SVM_I
					std::cout << "\n [FAIL] @" << i << ": (" << negative_mapped_data[i][0];
					for (int j = 1; j < Nv; j++)
						std::cout << "," << negative_mapped_data[i][j];
					std::cout << ")  \t add it to training set... ==>" << std::endl;
#endif
					//std::cout << RED << "x@" << i << " " << NORMAL;
					idx = i;
					return 0;
				}
			}

			// there should be no negatives misclassified.
#ifdef __PRT_SVM_I
			std::cout << "\n [PASS] @all";
#endif
			idx = -1;
			//std::cout << RED << "x@" << -1 << " " << NORMAL;
			return 0;
		}
};

#endif /* _SVM_I_H */
