#ifndef _ML_ALGO_H_
#define _ML_ALGO_H_
#include <iostream>
#include "states.h"
#include "equation.h"

class ML_Algo 
{
	public:
		ML_Algo() {};

		virtual int prepare_training_data(States*, int&, int&) = 0;

		virtual int train() = 0;

		virtual double predict_on_training_set() = 0;

		virtual int check_question_set(States&) = 0;

		virtual int get_converged(Equation*, int) = 0;

		friend std::ostream& operator << (std::ostream& out, const ML_Algo& mla) { return mla._print(out); }

		virtual std::ostream& _print(std::ostream& out) const {
			out << "ML-Algo: NULL";
			return out;
		};

		virtual int size() = 0;

		virtual Equation* roundoff(int&) = 0;

		virtual int predict(double*, int) = 0;
	private:
};

#endif /* _ML_Algo_H */
