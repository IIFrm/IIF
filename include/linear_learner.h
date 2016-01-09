#ifndef _LINEAR_
#define _LINEAR_
#include "config.h"
#include "base_learner.h"
#include "svm.h"


class LinearLearner: public BaseLearner {
	public:
		LinearLearner(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~LinearLearner();

		virtual int learn();

		int selectiveSampling(int randn, int exen, int type = 0, void* params = NULL);

	protected:
		SVM* svm;
		int max_iteration;
};

#endif
