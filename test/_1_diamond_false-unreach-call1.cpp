#include "iif.h"
#include <iostream>

int test_template(int* a)
{
	int x = 0;
	int y = a[0];
	iif_assume(y >= 0);
	while (x < 99) {
		recordi(y);
		if (y % 2 == 0)
		    x++;
		else 
		    x += 2;
	}
	recordi(y);
	iif_assert((x % 2) != (y % 2));
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(test_template, "temp_template") == false){ 
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
