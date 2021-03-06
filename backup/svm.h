#ifndef _SVM_H_
#define _SVM_H_
#include "ml_algo.h"
#include "svm_core.h"
#include "string.h"


class SVM : public MLalgo
{
	public:
		svm_parameter param;
		svm_problem problem;
		svm_model* last_model;
		int max_size;

		double* label; // [max_items * 2];
		double** data; // [max_items * 2];
		MState* pdata;

		int etimes;

	public:
		Polynomial* poly;
		svm_model* model;

	protected:
		int resize(int new_size) {
			if (new_size <= max_size) return 0;
			assert (new_size > max_size);
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
		/*
		inline Polynomial* getClassifier() {
			return poly;
		}

		inline svm_model* getModel() {
			return model;
		}

		inline int setClassifier(Polynomial* e) {
			*poly = *e;
			return 0;
		}
		*/

		SVM(int type = 0, void (*f) (const char*) = NULL, int size = 1000000) : max_size(size) {
			prepare_svm_parameters(param, type);
			if (f != NULL)
				svm_set_print_string_function(f);
			last_model = NULL;
			model = NULL;
			poly = NULL;

			data = new double*[max_size];
			pdata = new MState[max_size];
			label = new double[max_size];
			etimes = 0;
			for (int i = 0; i < max_size; i++)
				label[i] = -1;
			problem.l = 0;
			problem.x = (svm_node**)(data);
			problem.y = label;

#ifdef __DS_ENABLED
			problem.np = 0;
			problem.nn = 0;
#endif
		}

		~SVM() {
			//if (model != NULL) delete model;
#ifdef __DS_ENABLED
			problem.save_to_file("../tmp/svm.ds");
			std::cout << "save to file succeed. ../tmp/svm.ds\n";
#endif
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (last_model != NULL) svm_free_and_destroy_model(&last_model);
			if (poly != NULL) delete poly;
			if (data != NULL) delete []data;
			if (pdata != NULL) delete[]pdata;
			if (label != NULL) delete label;
		}

		void setDegree(int degree) {
			if (degree <= 0) return;
			param.degree = degree;
		}


		int makeTrainingSet(States* gsets, int& pre_psize, int& pre_nsize) {
			int cur_psize = gsets[POSITIVE].getSize();
			int cur_nsize = gsets[NEGATIVE].getSize();
			if (cur_psize + cur_nsize > max_size) 
				resize(cur_psize + cur_nsize);
			
#ifdef __PRT
			std::cout << "+[" << cur_psize - pre_psize << "|"
				<< cur_nsize - pre_nsize  << "] ==> ["
				<< cur_psize << "+|" << cur_nsize << "-]";
#endif

			//POINTER
			// prepare new training data set
			// training set & label layout:
			// data :  0 | positive states | negative states | ...
			// label:    | 1, 1, ..., 1, . | -1, -1, ..., -1, -1, -1, ...
			// move negative states from old OFFSET: [pre_positive_size] to new OFFSET: [cur_positive_size]
			memmove(data + cur_psize, data + pre_psize, pre_nsize * sizeof(double*));
			//memmove(pdata + cur_psize, pdata + pre_psize, pre_nsize * sizeof(MState));

			// add new positive states at OFFSET: [pre_positive_size]
			int cur_index = pre_psize + pre_nsize;
			for (int i = 0 ; i < cur_psize - pre_psize; i++) {
				mappingData(gsets[POSITIVE].values[pre_psize + i], pdata[cur_index + i], 4);
				data[pre_psize + i] = pdata[cur_index + i];
				label[pre_psize + i] = 1;
			}

			// add new negative states at OFFSET: [cur_positive_size + pre_negative_size]
			cur_index = cur_psize + pre_nsize;
			for (int i = 0 ; i < cur_nsize - pre_nsize; i++) {
				mappingData(gsets[NEGATIVE].values[pre_nsize + i], pdata[cur_index + i], 4);
				data[cur_index + i] = pdata[cur_index + i];
				label[cur_index + i] = -1;
			}

#ifdef __DS_ENABLED
			problem.np = cur_psize;
			problem.nn = cur_nsize;
#endif
			problem.l = cur_psize + cur_nsize;

			int ret = cur_psize + cur_nsize - pre_psize - pre_nsize;
			pre_psize = cur_psize;
			pre_nsize = cur_nsize;
			return ret;
		}

		int setEtimes(int et) {
			if ((et < 1) || (et > 4))
				return -1;
			etimes = et;
			switch (et) {
				case 1:
					return setDimension(Cv1to1);
				case 2:
					return setDimension(Cv1to2);
				case 3:
					return setDimension(Cv1to3);
				case 4:
					return setDimension(Cv1to4);
			}
			return -1;
		}

		bool mappingData(double* src, double* dst, int et = 4) {
			int index = 0;
			if (et >= 1) {
				for (int i = 0; i < Nv; i++) {
					dst[index++] = src[i];
				}
			}
			if (et >= 2) {
				for (int i = 0; i < Nv; i++) {
					for (int j = i; j < Nv; j++) {
						dst[index++] = src[i] * src[j];
					}
				}
			}
			if (et >= 3) {
				for (int i = 0; i < Nv; i++) {
					for (int j = i; j < Nv; j++) {
						for (int k = j; k < Nv; k++) {
							dst[index++] = src[i] * src[j] * src[k];
						}
					}
				}
			}
			if (et >= 4) {
				for (int i = 0; i < Nv; i++) {
					for (int j = i; j < Nv; j++) {
						for (int k = j; k < Nv; k++) {
							for (int l = k; l < Nv; l++) {
								dst[index++] = src[i] * src[j] * src[k] * src[l];
							}
						}
					}
				}
			}
			if (et >= 5) {
				std::cout << "Unsupported for 5 dimension up.\n";
				return false;
			}
			return true;
		}

		int train() {
			if (problem.y == NULL || problem.x == NULL) return -1;
			const char* error_msg = svm_check_parameter(&problem, &param);
			if (error_msg) { 
				std::cout << "ERROR: " << error_msg << std::endl; 
				return -1; 
			}

			if (model != NULL) {
				if (last_model != NULL)
					svm_free_and_destroy_model(&last_model);
				last_model = model;
			}
			/*if (poly != NULL) {
				delete poly;
				poly = NULL;
			}*/
			//std::cout << "checking point 1\n";
			//std::cout << std::endl << problem << std::endl;
			model = svm_train(&problem, &param);
			//std::cout << "\n\tmodel --> " << *model << std::endl;
			//std::cout << "checking point 2\n";
			if (poly == NULL) poly = new Polynomial();
			svm_model_visualization(model, poly);
			//svm_free_and_destroy_model(&model);
			//model = NULL;
			return 0;
		}

		double checkTrainingSet() {
			if (problem.l <= 0) return 0;
			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				//pass += (Polynomial::calc(*classifier, (double*)problem.x[i]) * problem.y[i] > 0) ? 1 : 0;
				pass += (predict((double*)problem.x[i]) * problem.y[i] >= 0) ? 1 : 0;
			}
			return static_cast<double>(pass) / problem.l;
		}

		int checkQuestionSet(States& qset) {
			if (classifier == NULL) return -1;
#ifdef __PRT
			std::cout << " [" << qset.traces_num() << "]";
#endif
			for (int i = 0; i < qset.p_index; i++) {
				int pre = -1, cur = 0;
#ifdef __PRT
				std::cout << ".";
#endif
				for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
					cur = Polynomial::calc(*classifier, qset.values[j]);
					//std::cout << ((cur >= 0) ? "+" : "-");
					if ((pre >= 0) && (cur < 0)) {
						// deal with wrong question trace.
						// Trace back to print out the whole trace and the predicted labels.
#ifdef __PRT
						std::cerr << RED << "\t[FAIL]\n \t  Predict wrongly on Question traces.\n";
						qset.dumpTrace(i);
#endif
						for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
							cur = Polynomial::calc(*classifier, qset.values[j]);
#ifdef __PRT
							std::cout << ((cur >= 0) ? "+" : "-");
#endif
						}
#ifdef __PRT
						std::cout << std::endl << WHITE;
#endif
						return -1;
					}
					pre = cur;
				}
			}
#ifdef __PRT
			std::cout << " [PASS]";
#endif
			return 0;
		}

		int converged (void* last_model, int num =1) {
			assert ((num == 1) || "SVM::get_converged: Unexpected equation number parameter.");
			if (last_model == NULL) return 1;
			Polynomial* pre_poly = (Polynomial*)last_model;
			return poly->is_similar(*pre_poly);
		}

		bool converged_model () {
			//return converged(last_model, 1);
			return model_converged(model, last_model);
		}

		friend std::ostream& operator << (std::ostream& out, const SVM& svm) {
			return svm._print(out);
		}

		std::ostream& _print(std::ostream& out) const {
			//out << "SVM-model: ";
			//out << *model << std::endl;
			//svm_model_visualization(model, *classifier);
			//out << *poly; // << std::endl;
			//svm_model_visualization(model, poly);
			out << poly->toString(); // << std::endl;
			/*Polynomial roundoff_poly = *poly;
			roundoff_poly.roundoff();
			out << "\n{" << roundoff_poly << "}";
			*/
			return out;
		}

		int getProblemSize() {
			return problem.l;
		}

		Polynomial* roundoff(int& num) {
			num = 1;
			Polynomial* newpoly = new Polynomial();
			poly->roundoff(*newpoly);
			return newpoly;
		}

		int predict(double* v) {
			if (model == NULL) return -2;
			if (v == NULL) return -2;
			double res = svm_predict(model, (svm_node*)v); 

			if (res >= 0) return 1;
			else return -1;
		}
};

#endif /* _SVM_H */
