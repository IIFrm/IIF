#ifndef _RBF_
#define _RBF_
#include "config.h"
#include "base_learner.h"
#include "svm.h"
#include "classifier.h"


class RbfLearner: public BaseLearner {
	public:
		RbfLearner(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~RbfLearner();

		virtual int learn();

		int selectiveSampling(int randn, int exen, int type = 0, void* params = NULL);

		virtual std::string invariant();

	protected:
		SVM* svm;
		int max_iteration;
		Classifier* cl;
};

#endif
