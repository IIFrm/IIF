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
		//svm_model* last_model;
		int max_size;

		MState* raw_mapped_data;
		double** data; // [max_items * 2];
		double* label; // [max_items * 2];
		int etimes;
		int kernel;

		svm_model* model;

	protected:
		int resize(int new_size) {
			if (new_size <= max_size) return 0;
			assert (new_size > max_size);
			int valid_size = problem.l;

			// enlarge max_size exponentially to cover all the data.
			while (new_size >= max_size) max_size *= 2;

			double ** new_data = new double*[max_size];
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
#ifdef __TRAINSET_SIZE_RESTRICTED
		SVM(int type = 0, void (*f) (const char*) = NULL, int size = 1000000) : max_size(size) {
#else
		SVM(int type = 0, void (*f) (const char*) = NULL, int size = trainsetsize+1) : max_size(size) {
#endif
			prepare_svm_parameters(&param, type);
			if (f != NULL)
				svm_set_print_string_function(f);
			model = NULL;

			data = new double*[max_size];
			raw_mapped_data = new MState[max_size];
			label = new double[max_size];
			etimes = 0;
			for (int i = 0; i < max_size; i++)
				label[i] = -1;
			problem.l = 0;
			problem.x = (svm_node**)(data);
			problem.y = label;
			etimes = 1;
			kernel = 0;

#ifdef __DS_ENABLED
			problem.np = 0;
			problem.nn = 0;
#endif
		}

		void setKernel(int kn) {
			kernel = kn;
		}

		~SVM() {
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (raw_mapped_data != NULL) delete[]raw_mapped_data;
			if (data != NULL) delete []data;
			if (label != NULL) delete label;
		}


		int makeTrainingSet(States* gsets, int& pre_psize, int& pre_nsize) {
			int cur_psize = gsets[POSITIVE].getSize();
			int cur_nsize = gsets[NEGATIVE].getSize();
			if (cur_psize + cur_nsize > max_size) 
				resize(cur_psize + cur_nsize);

#ifdef __PRT
			std::cout << "++[" << cur_psize - pre_psize << "|"
				<< cur_nsize - pre_nsize  << "] ==> ["
				<< cur_psize << "+|" << cur_nsize << "-]";
#endif

			int ret = cur_psize + cur_nsize - pre_psize - pre_nsize;
#ifdef __TRAINSET_SIZE_RESTRICTED
			int pstart = cur_psize > trainsetsize? cur_psize - trainsetsize: 0;
			int plength = cur_psize - pstart;
			for (int i = 0; i < plength; i++) {
				mappingData(gsets[POSITIVE].values[pstart + i], raw_mapped_data[i], 4);
				data[i] = raw_mapped_data[i];
				label[i] = 1;
			}
			int nstart = cur_nsize > trainsetsize? cur_nsize - trainsetsize: 0;
			int nlength = cur_nsize - nstart;
			for (int i = 0; i < nlength; i++) {
				mappingData(gsets[NEGATIVE].values[nstart + i], raw_mapped_data[plength + i], 4);
				data[plength + i] = raw_mapped_data[plength + i];
				label[plength + i] = -1;
			}
			cur_psize = plength;
			cur_nsize = nlength;
			std::cout << RED << " RESTRICTED2[" << cur_psize << "+|" << cur_nsize << "-]" << NORMAL;
			ret = plength + nlength;
#else
			//POINTER
			// prepare new training data set
			// training set & label layout:
			// data :  0 | positive states | negative states | ...
			// label:    | 1, 1, ..., 1, . | -1, -1, ..., -1, -1, -1, ...
			// move negative states from old OFFSET: [pre_positive_size] to new OFFSET: [cur_positive_size]
			memmove(data + cur_psize, data + pre_psize, pre_nsize * sizeof(double*));
			//memmove(raw_mapped_data + cur_psize, raw_mapped_data + pre_psize, pre_nsize * sizeof(MState));

			// add new positive states at OFFSET: [pre_positive_size]
			int cur_index = pre_psize + pre_nsize;
			for (int i = 0 ; i < cur_psize - pre_psize; i++) {
				mappingData(gsets[POSITIVE].values[pre_psize + i], raw_mapped_data[cur_index + i], 4);
				data[pre_psize + i] = raw_mapped_data[cur_index + i];
				label[pre_psize + i] = 1;
			}

			// add new negative states at OFFSET: [cur_positive_size + pre_negative_size]
			cur_index = cur_psize + pre_nsize;
			for (int i = 0 ; i < cur_nsize - pre_nsize; i++) {
				mappingData(gsets[NEGATIVE].values[pre_nsize + i], raw_mapped_data[cur_index + i], 4);
				data[cur_index + i] = raw_mapped_data[cur_index + i];
				label[cur_index + i] = -1;
			}
#endif

#ifdef __DS_ENABLED
			problem.np = cur_psize;
			problem.nn = cur_nsize;
#endif

			//problem.l = cur_psize + cur_nsize;
			problem.l = cur_psize + cur_nsize;

			pre_psize = cur_psize;
			pre_nsize = cur_nsize;
			//std::cout << "makeTrainingSet => " << ret << "\n";
			return ret;
		}

		int train() {
			if (problem.y == NULL || problem.x == NULL) return -1;
			const char* error_msg = svm_check_parameter(&problem, &param);
			if (error_msg) { 
				std::cout << "ERROR: " << error_msg << std::endl; 
				return -1; 
			}

			int res = 0;
			if (kernel == 0) {
				res = trainLinear();
			} else {
				res = trainPoly();
			}
			/*if (res == 0)
				cl.roundoff();
				*/
			return res;
			/*
			   Polynomial poly;
			   while (etimes <= 3) {
			   setEtimes(etimes);
			   model = svm_train(&problem, &param);
			   svm_model_visualization(model, &poly);
			   double pass_rate = checkTrainingSet();

			   if (pass_rate == 1)
			   break;
			   etimes++;
			   }
			   if (etimes >= 4) return -1;
			//svm_free_and_destroy_model(&model);
			//model = NULL;
			cl = poly;
			return 0;
			*/
		}


		double checkTrainingSet() {
			if (problem.l <= 0) return 0;
			int pass = 0;
#ifdef __PRT_POLYSVM
			std::cout << RED << BOLD << " PREDICT WRONGLY>>> >>" << NORMAL << BLUE;
#endif
			for (int i = 0; i < problem.l; i++) {
#ifdef __PRT_POLYSVM
				double predict_result = predict((double*)problem.x[i]);
				if (predict_result * problem.y[i] < 0) {
					std::cout << RED << BOLD << "([" << problem.x[i][0];
					for (int j = 1; j < Nv; j++)
						std::cout << "," << problem.x[i][j];
					std::cout << "]" << problem.y[i] << "->" << predict_result << ")  >>";
				}
#endif
				pass += (predict((double*)problem.x[i]) * problem.y[i] >= 0) ? 1 : 0;
			}
#ifdef __PRT_POLYSVM
			std::cout << RED << BOLD << " >>>>>END CHECKING\n" << NORMAL;
#endif
			return static_cast<double>(pass) / problem.l;
		}

		int checkQuestionTraces(States& qset) {
#ifdef __PRT
			std::cout << " [" << qset.traces_num() << "]";
#endif
			for (int i = 0; i < qset.p_index; i++) {
				int pre = -1, cur = 0;
#ifdef __PRT
				std::cout << ".";
#endif
				for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
					cur = predict(qset.values[j]);
					//std::cout << ((cur >= 0) ? "+" : "-");
					if ((pre >= 0) && (cur < 0)) {
						// deal with wrong question trace.
						// Trace back to print out the whole trace and the predicted labels.
#ifdef __PRT
						std::cerr << RED << "\t[FAIL]\n \t  Predict wrongly on Question traces.\n";
						qset.dumpTrace(i);
#endif
						for (int j = qset.t_index[i]; j < qset.t_index[i + 1]; j++) {
							cur = predict(qset.values[j]);
#ifdef __PRT
							std::cout << ((cur >= 0) ? "+" : "-");
#endif
						}
#ifdef __PRT
						std::cout << std::endl << NORMAL;
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

		bool converged (Classifier& pre_cl) {
			if (pre_cl.size <= 0) return false;
			return cl[0]->isSimilar(*pre_cl[0]);
		}


		friend std::ostream& operator << (std::ostream& out, const SVM& svm) {
			return svm._print(out);
		}

		std::ostream& _print(std::ostream& out) const {
			//svm_model_visualization(model, *classifier);
			//out << *poly; // << std::endl;
			//svm_model_visualization(model, poly);
			//out << cl << std::endl;
			out << cl[0]->toString(); // << std::endl;
			return out;
		}

		int getProblemSize() {
			return problem.l;
		}

		int predict(double* v) {
			if (model == NULL) return -2;
			if (v == NULL) return -2;
			double res = svm_predict(model, (svm_node*)v); 

			return res;
			if (res >= 0) return 1;
			else return -1;
		}

		int trainLinear() {
			Polynomial poly;
			model = svm_train(&problem, &param);
			svm_model_visualization(model, &poly);
			cl = poly;
			return 0;
		}

		int trainPoly() {
			Polynomial poly;
#ifdef __PRT_POLYSVM
			std::cout << RED  << "\ntrying from etimes = " << etimes << " $$$$$$ "<< NORMAL;
#endif
			while (etimes <= 4) {
				setEtimes(etimes);
				model = svm_train(&problem, &param);
				svm_model_visualization(model, &poly);
				double pass_rate = checkTrainingSet();
#ifdef __PRT_POLYSVM
				std::cout << BLUE << "   [" << etimes << "] " << pass_rate*100 << "% --> " << NORMAL << poly << std::endl;
#endif
				if (pass_rate == 1)
					break;
				etimes++;
			}
			if (etimes > 4) return -1;
			cl = poly;
			return 0;
		}
};

#endif /* _SVM_H */
