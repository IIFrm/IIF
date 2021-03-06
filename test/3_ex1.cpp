#include "iif.h"

static int nondet() {
	return rand() % 4;
}

int ex1(int* a) {
	int x;
	int y;
	int xa = a[0];
	int ya = a[1];
	int loop_times = a[2] % 10;
	int branch = 1;

	iif_assume(xa + 2 * ya >= 0);
	while (loop_times-- > 0) {
		recordi(xa, ya, loop_times);
		x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (branch) y = y + x;
		else y = y - x;
		branch = 1 - branch;

		xa = x - 2 * y;
		ya = 2 * x + y;
	}
	recordi(xa, ya, loop_times);
	iif_assert(xa + 2 * ya >= 0);
	return 0;
}

int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(ex1, "ex1") == false) {
		return -1;
	} 

#ifdef linux
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	//alarm(60);
#endif
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
