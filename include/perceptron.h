#ifndef _PERCEPTRON_H_
#define _PERCEPTRON_H_

#include "config.h"
#include "instrumentation.h"
#include "color.h"
#include "ml_algo.h"

class Perceptron : public ML_Algo
{
	private:
		const int max_size;
	public:
		Equation* main_equation;
		double training_label[max_items * 2];
		double* training_set[max_items * 2];
		int length;

		Perceptron(void (*f) (const char*) = NULL, int max_size = 10000);

		virtual ~Perceptron(); 

		virtual int prepare_training_data(States* gsets, int& pre_positive_size, int& pre_negative_size);

		virtual int train();

		virtual double predict_on_training_set();

		virtual int check_question_set(States& qset);

		friend std::ostream& operator << (std::ostream& out, const Perceptron&);

		virtual int size();

		virtual Equation* roundoff(int& num);

		virtual int predict(double* v, int label = 0);

	private:
		Equation* perceptron_train();
		
};

#endif /* _PERCEPTRON_H */
