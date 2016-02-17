#ifndef _KERNEL_
#define _KERNEL_
#include "config.h"
#include "base_learner.h"
#include "svm.h"


class KernelLearner: public BaseLearner {
	public:
		KernelLearner(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~KernelLearner();

		virtual int learn();

		int selectiveSampling(int randn, int exen, int type = 0, void* params = NULL);

		virtual std::string invariant();

	protected:
		SVM* svm;
		int max_iteration;
};

#endif
