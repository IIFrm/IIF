#include "iif.h"
#include <iostream>

int func(int* a)
{
	int x;
	x = a[0];
	iif_assume(x % 2 == 0);
	while (x >= 10) {
		recordi(x);
		x += 2;
	}
	recordi(x);
	iif_assert(x % 2 == 0);
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(func, "fuc") == false){ 
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
