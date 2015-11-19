#include "perceptron.h"

Perceptron::Perceptron(void (*f) (const char*)) {
	length = 0;
	main_equation = NULL;
	for (int i = 0; i < 2 * max_items; i++)
		training_label[i] = -1;
}

Perceptron::~Perceptron() {
	//if (problem.y != NULL)
	//	delete problem.y;
	// here we should check x[i] for each.
	// be careful about whether it is imported from double trace set or int trace set.
	// these two cases should be handled separatly.
}

int Perceptron::prepare_training_data(States* gsets, int& pre_positive_size, int& pre_negative_size) {
	int cur_positive_size = gsets[POSITIVE].size();
	int cur_negative_size = gsets[NEGATIVE].size();

	std::cout << "+[";
	std::cout << cur_positive_size - pre_positive_size << "|";
	std::cout << cur_negative_size - pre_negative_size << "";
	std::cout << "] ==> [";
	std::cout << cur_positive_size << "+|";
	std::cout << cur_negative_size << "-";
	std::cout << "]";

	//double** training_set = (double**)(problem.x);
	//double* training_label = (double*)(problem.y);
	// prepare new training data set
	// training set & label layout:
	// data :  0 | positive states | negative states | ...
	// label:    | 1, 1, ..., 1, . | -1, -1, ..., -1, -1, -1, ...
	// move the negative states from old OFFSET: [pre_positive_size] to new OFFSET: [cur_positive_size]
	memmove(training_set + cur_positive_size, training_set + pre_positive_size, pre_negative_size * sizeof(double*));
	// add new positive states at OFFSET: [pre_positive_size]
	for (int i = pre_positive_size; i < cur_positive_size; i++) {
		training_set[i] = gsets[POSITIVE].values[i];
		training_label[i] = 1;
	}
	// add new negative states at OFFSET: [cur_positive_size + pre_negative_size]
	for (int i = pre_negative_size; i < cur_negative_size; i++) {
		training_set[cur_positive_size + i] = gsets[NEGATIVE].values[i];
		//training_label[cur_positive_size + i] = -1;
	}
	length = cur_positive_size + cur_negative_size;
	pre_positive_size = cur_positive_size;
	pre_negative_size = cur_negative_size;
	return 0;
}


int Perceptron::train() {
	if (length <= 0)
		return -1;
	main_equation = perceptron_train();
	return 0;
}


double Perceptron::predict_on_training_set() {
	if (length <= 0) return 0;
	int pass = 0;
	for (int i = 0; i < length; i++) {
		pass += (Equation::calc(main_equation, (double*)training_set[i]) * training_label[i] > 0) ? 1 : 0;
	}
	return static_cast<double>(pass) / length;
}


int Perceptron::check_question_set(States& qset) {
	if (main_equation == NULL) return -1;
	std::cout << " [" << qset.traces_num() << "]";
	for (int i = 0; i < qset.p_index; i++) {
		int pre = -1, cur = 0;
		std::cout << ".";
		//std::cout << "\t\t" << i << ">";
		//gsets[QUESTION].print_trace(i);
		for (int j = qset.index[i]; j < qset.index[i + 1]; j++) {
			cur = Equation::calc(main_equation, qset.values[j]);
			//std::cout << ((cur >= 0) ? "+" : "-");
			if ((pre >= 0) && (cur < 0)) {
				// deal with wrong question trace.
				// Trace back to print out the whole trace and the predicted labels.
				std::cerr << "\t\t[FAIL]\n \t  Predict wrongly on Question traces." << std::endl;
				qset.print_trace(i);
				for (int j = qset.index[i]; j < qset.index[i + 1]; j++) {
					cur = Equation::calc(main_equation, qset.values[j]);
					std::cout << ((cur >= 0) ? "+" : "-");
				}
				std::cout << std::endl;
				return -1;
			}
			pre = cur;
		}
		//std::cout << "END" << std::endl;
	}
	std::cout << " [PASS]";
	return 0;
}


std::ostream& operator << (std::ostream& out, const Perceptron& perceptron) {
	out << "Perceptron: ";
	out << *perceptron.main_equation; // << std::endl;
	return out;
}


int Perceptron::size()
{
	return length;
}


Equation* Perceptron::roundoff(int& num)
{
	num = 1;
	Equation* equs = new Equation[1];
	main_equation->roundoff(equs[0]);
	return equs;
}

int Perceptron::predict(double* v, int label) {
	if (main_equation == NULL) return -2;
	if (v == NULL) return -2;
	double res = Equation::calc(main_equation, v);
	
	// This is for debug info, should be removed by release time.
	if (label == 1) { 
		std::cout << "(" << v[0];
		for (int j = 1; j < VARS; j++) 
			std::cout << "," << v[j];
		std::cout << ")";
		set_console_color(std::cout, GREEN);
		if (res >= 0) std::cout << "+";
		else std::cout << "-";
		unset_console_color(std::cout);
	}
	
	if (res >= 0) return 1;
	else return -1;
}

Equation* Perceptron::perceptron_train()
{
	Equation* equs = new Equation[1]();
	bool error = true;
	//std::cout << equs[0] << std::endl;
	//std::cout << length << std::endl;
	//for (int i = 0; i < length; i++) 
	//	std::cout << i << ")"<< training_set[i][0] << ","<< training_set[i][1] << "->" << training_label[i] << "\t||";
	while (error) {
		error = false;
		std::cout << "*";
		int start = rand() % length;
		for (int i = start; i < length; i++) {
			double predict_label = Equation::calc(&equs[0], training_set[i]);
			if (predict_label == 0) predict_label = 1;
			
			//std::cout << i << ")";
			//std::cout << predict_label << " ";
			//std::cout << training_label[i] << "\t||";

			if (predict_label * training_label[i] < 0) {
				// there is a point mis classified
				// we update theta and theta0 here.
				//std::cout << "update theta and theta0..." << std::endl;
				error = true;
				for (int j = 0; j < VARS; j++)
					equs[0].theta[j] += training_label[i] * training_set[i][j];
				equs[0].theta0 += training_label[i];
			}
		}
		for (int i = 0; i < start; i++) {
			double predict_label = Equation::calc(&equs[0], training_set[i]);
			if (predict_label == 0) predict_label = 1;
			
			//std::cout << i << ")";
			//std::cout << predict_label << " ";
			//std::cout << training_label[i] << "\t||";

			if (predict_label * training_label[i] < 0) {
				// there is a point mis classified
				// we update theta and theta0 here.
				//std::cout << "update theta and theta0..." << std::endl;
				error = true;
				for (int j = 0; j < VARS; j++)
					equs[0].theta[j] += training_label[i] * training_set[i][j];
				equs[0].theta0 += training_label[i];
			}
		}
	}
	return equs;
}
