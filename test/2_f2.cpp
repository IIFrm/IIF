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
	if (argc >= 3) {
		minv = atoi(argv[1]);
		maxv = atoi(argv[2]);
	}

	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(f2, "f2") == false) {
		return -1;
	}

	IIF_svm_learn isl(gsets, target_program);
	if (isl.learn() == 0) {
		std::cout << "END.\n";
		return 0;
	}

	std::cout << "TRY SVM-I method again...\n";
	IIF_svm_i_learn isil(gsets, target_program);
	if (isil.learn() == 0) {
		std::cout << "END.\n";
		return 0;
	}

	std::cout << "TRY other method again...\n";
	return 0;
}
