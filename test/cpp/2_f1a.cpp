#include "iif.h"

int f1a(int* a) {
	int x = a[0];
	int y = a[1];

	iif_assume(x + y >= 0);
	while (x >= 0) {
		recordi(x, y);
		x--;
		y++;
	}
	recordi(x, y);
	iif_assert(y > 0);
	return 0;
}

int main(int argc, char** argv)
{
	LearnerPipeline lp(f1a, "f1a");
	lp.addLearner("linear").addLearner("conjunctive");
	return lp.learn();
}
