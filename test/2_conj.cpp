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
	int loop_times = a[1] % 10;
	iif_assume((x >= 0) && (x <= 50));
	while (loop_times-- > 0) {
		recordi(x, loop_times);
		if (x <= 0) {
			x ++;
		} else {
			x--;
		}
	}
	recordi(x, loop_times);
	iif_assert((x >= 0) && (x <= 50));
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(conj, "conj") == false){ 
		return -1;
	} 

#ifdef linux
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(60);
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
