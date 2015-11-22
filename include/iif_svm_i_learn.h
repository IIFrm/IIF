#ifndef _IIF_SVM_I_LEARN_
#define _IIF_SVM_I_LEARN_
#include "config.h"
#include "iif_learn.h"
#include "ml_algo.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class IIF_svm_i_learn: public IIF_learn {
	public:
		IIF_svm_i_learn(States* gsets, int (*func)(int*), int max_iteration = max_iter);

		IIF_svm_i_learn();

		virtual int learn();

	protected:
		SVM_I* svm_i;
		int max_iteration;
};

#endif
