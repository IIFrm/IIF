#include "iif.h"

extern int minv, maxv;
int main(int argc, char** argv)
{
	if (argc >= 3) {
		minv = atoi(argv[1]);
		maxv = atoi(argv[2]);
	}

	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	register_program(conj, "conj");
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

