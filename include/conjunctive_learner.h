#ifndef _CONJUNCTIVE_
#define _CONJUNCTIVE_
#include "config.h"
#include "base_learner.h"
#include "ml_algo.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class ConjunctiveLearner: public BaseLearner {
	public:
		ConjunctiveLearner(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~ConjunctiveLearner();

		virtual int learn();

	protected:
		int programExecutor(int randn, int exen, int type, void* paras);
		SVM_I* svm_i;
		int max_iteration;
};

#endif
