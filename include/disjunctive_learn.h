#ifndef _DISJUNCTIVE_
#define _DISJUNCTIVE_
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

class DisjunctiveLearn: public LearnBase {
	public:
		DisjunctiveLearn(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter, int K = 8);

		DisjunctiveLearn();

		virtual int learn();

	protected:

		int k_means(int gset_index, int k);
		bool linear_separable(int positive_label, int negative_label);
		int *positive_group, *negative_group;
		double (*checkset)[VARS];

		SVM* svm;
		int max_iteration;
		int K;
};

#endif
