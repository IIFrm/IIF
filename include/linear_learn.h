#ifndef _LINEAR_
#define _LINEAR_
#include "config.h"
#include "learn_base.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class LinearLearn: public LearnBase {
	public:
		LinearLearn(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		LinearLearn();

		virtual int learn();

	protected:
		SVM* svm;
		int max_iteration;
};

#endif
