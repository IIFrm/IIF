#ifndef _POLY_
#define _POLY_
#include "config.h"
#include "base_learner.h"
#include "svm.h"


class PolyLearner: public BaseLearner {
	public:
		PolyLearner(States* gsets, int (*func)(int*) = target_program, int max_iteration = max_iter);

		~PolyLearner();

		virtual int learn();

		int selectiveSampling(int randn, int exen, int type = 0, void* params = NULL);

		virtual std::string invariant();

	protected:
		SVM* svm;
		int max_iteration;
};

#endif
