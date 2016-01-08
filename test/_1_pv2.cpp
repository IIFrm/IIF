#include "iif.h"
#include <iostream>


int pv2(int* a)
{
	int x = a[0];
	int y = x;
	iif_assume((x >= 0) && (x <= 50));
	while (true) {
		recordi(x);
		if (x <= 50) {
			y++;
		}
		else 
			y--;
		if (y < 0) {
			break;
		}
		x++;
	}
	recordi(x);
	iif_assert(x == 102);
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(pv2, "pv2") == false){ 
		return -1;
	} 

#ifdef linux
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(120);
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
