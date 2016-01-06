#ifndef _IIF_DISJUNCTIVE_SVM_LEARN_
#define _IIF_DISJUNCTIVE_SVM_LEARN_
#include "config.h"
#include "iif_learn.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class IIF_disjunctive_svm_learn: public IIF_learn {
	public:
		IIF_disjunctive_svm_learn(States* gsets, int (*func)(int*), int max_iteration = max_iter, int K = 8);

		IIF_disjunctive_svm_learn();

		virtual int learn();

	protected:

		int k_means(States& s, int k);

		SVM* svm;
		int max_iteration;
		int K;
};

#endif
