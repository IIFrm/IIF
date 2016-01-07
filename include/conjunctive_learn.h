#ifndef _CONJUNCTIVE_
#define _CONJUNCTIVE_
#include "config.h"
#include "learn_base.h"
#include "ml_algo.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class ConjunctiveLearn: public LearnBase {
	public:
		ConjunctiveLearn(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		ConjunctiveLearn();

		~ConjunctiveLearn();

		virtual int learn();

	protected:
		SVM_I* svm_i;
		int max_iteration;
};

#endif
