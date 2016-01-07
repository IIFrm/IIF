#include "iif.h"
#include <iostream>

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
	LinearLearn isl(gsets, target_program);
	if (isl.learn() == 0) { return 0;}

	std::cout << "TRY SVM-I method ...\n";
	ConjunctiveLearn isil(gsets, target_program);
	if (isil.learn() == 0) { return 0;}

	//std::cout << "TRY other method again...\n";
	shutdownSystem(gsets);
	return 0;
}
