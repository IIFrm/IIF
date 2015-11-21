#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"
#include "iif_learn.h"
#include "iif_svm_learn.h"
#include "iif_svm_i_learn.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

int minv = -100, maxv = 100;

States global_states_sets[4];
States* gsets = &global_states_sets[1];

int main(int argc, char** argv)
{
	if (argc >= 3) {
		minv = atoi(argv[1]);
		maxv = atoi(argv[2]);
	}

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

