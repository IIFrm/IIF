#include "iif.h"
#include <iostream>

int conj(int* a)
{
	int x;
	x = a[0];
	int loop_times = 10;
	iif_assume((x >= 0) && (x <= 50));
	while (loop_times-- > 0) {
		recordi(x);
		if (x == 50) {
			x --;
		} else if (x == 0) {
			x ++;
		} else {
			x += rand() % 3 - 1;
		}
	}
	recordi(x);
	iif_assert((x >= 0) && (x <= 50));
	return 0;
}


int main(int argc, char** argv)
{
	LearnerPipeline lp(conj, "conj");
	lp.addLearner("linear").addLearner("conjunctive");
	return lp.learn();
}
