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

		MState* raw_mapped_data;
		double** data; // [max_items * 2];
		double* label; // [max_items * 2];

	public:
		//Polynomial* poly;
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
			raw_mapped_data = new MState[max_size];
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
#ifdef __DS_ENABLED
			problem.save_to_file("../tmp/svm.ds");
			std::cout << "save to file succeed. ../tmp/svm.ds\n";
#endif
			if (model != NULL) svm_free_and_destroy_model(&model);
			if (last_model != NULL) svm_free_and_destroy_model(&last_model);
			if (poly != NULL) delete poly;
			if (raw_mapped_data != NULL) delete[]raw_mapped_data;
			if (data != NULL) delete []data;
			if (label != NULL) delete label;
		}

		/*void setDegree(int degree) {
			if (degree <= 0) return;
			param.degree = degree;
		}*/


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

			//std::cout << std::endl << problem << std::endl;
			model = svm_train(&problem, &param);
			//std::cout << "\n\tmodel --> " << *model << std::endl;
			if (poly == NULL) poly = new Polynomial();
			svm_model_visualization(model, poly);
			cl = *poly;
			//svm_free_and_destroy_model(&model);
			return 0;
		}

		double checkTrainingSet() {
			if (problem.l <= 0) return 0;
			int pass = 0;
			for (int i = 0; i < problem.l; i++) {
				pass += (predict((double*)problem.x[i]) * problem.y[i] >= 0) ? 1 : 0;
			}
			return static_cast<double>(pass) / problem.l;
		}

		int converged (void* last_model, int num =1) {
			return converged_model();
			assert ((num == 1) || "SVM::get_converged: Unexpected equation number parameter.");
			if (last_model == NULL) return 1;
			Polynomial* pre_poly = (Polynomial*)last_model;
			return poly->is_similar(*pre_poly);
		}

		bool converged_model () {
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
			out << cl << std::endl;
			out << poly->toString(); // << std::endl;
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
