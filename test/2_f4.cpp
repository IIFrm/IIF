#include "iif.h"

int f2(int* a) {
	int x = a[0];
	int y = a[1];
	iif_assume((x + y >= 0) && (x - y >= 0));
	while (y > 0) {
		recordi(x, y);
		x++;
		y--;
	}

	recordi(x, y);
	iif_assert((x + y >= 0) && (x - y >= 0));
	return 0;
}


int main(int argc, char** argv)
{
	States* gsets = initSystem(f2, "f2");

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
