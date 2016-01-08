#ifndef _LINEAR_
#define _LINEAR_
#include "config.h"
#include "learn_base.h"
#include "svm.h"


class LinearLearn: public LearnBase {
	public:
		LinearLearn(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~LinearLearn();

		virtual int learn();

	protected:
		int programExecutor(int randn, int exen, int type = 0, void* paras = NULL);
		SVM* svm;
		int max_iteration;
};

#endif
