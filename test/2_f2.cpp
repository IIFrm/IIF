#include "iif.h"

static int nondet() {
	return rand() % 4;
	return rand() % 20;
}

int f2(int* a) {
	int x = a[0];
	int y = a[1];
	int z;
	int w;

	iif_assume(3 * x > y - 1);
	while (nondet()) {
		recordi(x, y);
		if (nondet()) {x++; y = y+2;}
		else if (nondet()) {
			if (x >= 4) {x++; y = y+3; z = z+10; w = w+10;}
		}
		else if (x >= z && w >= y+1) {x = -x; y = -y; }

	}
	recordi(x, y);
	iif_assert(3 * x > y - 1);
	return 0;
}

int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(f2, "f2") == false) {
		return -1;
	} 

	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(60);
	std::cout << "TRY SVM method ...\n";
	IIF_svm_learn isl(gsets, target_program);
	if (isl.learn() == 0) {  
		return 0;
	}

	std::cout << "TRY SVM-I method ...\n";
	IIF_svm_i_learn isil(gsets, target_program);
	if (isil.learn() == 0) {	
		return 0;
	}

	//std::cout << "TRY other method again...\n";
	return 0;
}
