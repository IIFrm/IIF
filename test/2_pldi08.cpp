//#include "header.h"
#include "iif.h"


int pldi08(int* a){
	int x = a[0];
	int y = a[1];
	iif_assume (x < 0);
	while (x < 0) {
		recordi(x, y);
     x = x + y;
     y++;
	}

	recordi(x, y);
	iif_assert (y >= 0);
	return 0;
}


int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(pldi08, "pldi08") == false) { 
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
