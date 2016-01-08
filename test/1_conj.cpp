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
	States* gsets = initSystem(conj, "conj");

	std::cout << "TRY SVM method ...\n";
	LinearLearner ll(gsets);
	if (ll.learn() == 0) return destroySystem(gsets); 

	std::cout << "TRY SVM-I method ...\n";
	ConjunctiveLearner cl(gsets);
	if (cl.learn() == 0) return destroySystem(gsets); 

	//std::cout << "TRY other method again...\n";
	destroySystem(gsets);
	return -1;
}
