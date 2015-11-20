#ifndef _SVM_I_H_
#define _SVM_I_H_

#include "svm.h"
#include "color.h"
#include <iostream>


class SVM_I : public SVM
{
private:
	const int max_equ;

public:
	svm_model* model;
	Equation* equations;
	int equ_num;
	svm_parameter param;
	States* negatives; 

	SVM_I(void(*f) (const char*) = NULL, Equation* eq = NULL, int equ = 16);

	~SVM_I();

	virtual int prepare_training_data(States* gsets, int& pre_positive_size, int& pre_negative_size);

	int train();

	double predict_on_training_set();

	virtual int check_question_set(States& qset);

	friend std::ostream& operator << (std::ostream& out, const SVM_I& svm_i);

	virtual std::ostream& _print(std::ostream& out) const;

	int size();

	virtual Equation* roundoff(int& num);

	virtual int predict(double* v, int label = 0);

private:
	double check_postives_and_one_negative();

	int get_misclassified(int& idx); // negative points may be misclassified.
};

#endif /* _SVM_I_H */
