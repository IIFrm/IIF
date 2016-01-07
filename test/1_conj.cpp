#include "iif.h"
#include <iostream>

static int nondet() {
	return rand() % 4;
	return rand() % 20;
}

int conj(int* a)
{
	int x;
	x = a[0];
	int loop_times = 10;
	iif_assume((x >= 0) && (x <= 50));
	while (loop_times-- > 0) {
		recordi(x);
		if (x >= 50) {
			x --;
		}
		else if (x <= 0) {
			x ++;
		}
		else {
			x += rand() %3 - 1;
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
	LinearLearn isl(gsets, target_program);
	if (isl.learn() == 0) { return 0;}

	std::cout << "TRY SVM-I method ...\n";
	ConjunctiveLearn isil(gsets, target_program);
	if (isil.learn() == 0) { return 0;}

	//std::cout << "TRY other method again...\n";
	shutdownSystem(gsets);
	return 0;
}
