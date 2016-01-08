#include "iif.h"
#include <iostream>

int test_template(int* a)
{
	int x;
	x = a[0];
	iif_assume(x > 0);
	while (x >= 2) {
		recordi(x);
		if (x % 2 == 0) 
			x = x / 2;
		else
			x = 3 * x + 1;
	}
	recordi(x);
	iif_assert(x == 1);
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(test_template, "temp_template") == false){ 
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
