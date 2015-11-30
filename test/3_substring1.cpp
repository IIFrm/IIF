#include "iif.h"

int substring1(int* a) {
	int j = a[2];
	j = 0;
	int i = a[0];
	int k = a[1];

	iif_assume ((i >= 0) && (i <= k) && (k >= 0) && (k <= 100)); 
	while (i < k) {
		recordi(i, k, j);
		i++;	
		j++;
	}
	recordi(i, k, j);
	iif_assert(j < 101);
	return 0;
}

int main(int argc, char** argv)
{
	States global_states_sets[4];
	States* gsets = &global_states_sets[1];

	if (register_program(substring1, "substring1") == false) {
		return -1;
	} 

#ifdef linux
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(360);
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
