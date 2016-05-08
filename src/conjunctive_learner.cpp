#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "color.h"
#include "polynomial.h"
#include "conjunctive_learner.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

static void print_null(const char *s) {}

ConjunctiveLearner::ConjunctiveLearner(States* gsets, int (*func)(int*), int max_iteration) 
	: BaseLearner(gsets, func) { 
	svm_i = new SVM_I(0, print_null);
	this->max_iteration = max_iteration;
}

ConjunctiveLearner::~ConjunctiveLearner() { 
	if (svm_i != NULL)
		delete svm_i;
}

int ConjunctiveLearner::learn()
{
	std::cout << YELLOW << ">>>> Conjunctive Learner-----------------------\n" << NORMAL;  
	Solution inputs;
	srand(time(NULL)); // initialize seed for rand() function

	int rnd;
	bool lastSimilar = false;
	bool converged = false;
	Classifier pre_cl;
	int pre_psize = 0, pre_nsize = 0; // , pre_question_size = 0;
	int base_maxv = maxv;
	int base_minv = minv;
	double pass_rate = 1;

	for (rnd = 1; ((rnd <= max_iteration) && (pass_rate >= 1)); rnd++) {
		int zero_times = 0;
		//std::cout << "[" << rnd << "]";
		int nexe = (rnd == 1) ? Nexe_init : Nexe_after;
#ifdef __PRT
		int step = 1;
		std::cout << RED << "[" << rnd << "]" << NORMAL;
		std::cout << "SVM-I----------------------------------------------------------"
			"------------------------------------------------";
		std::cout << "\n\t(" << step++ << ") execute programs... [" << nexe + Nexe_rand << "] ";
#else
		std::cout << RED << "[" << rnd;
#endif
init_svm_i:
		selectiveSampling(Nexe_rand, nexe, &pre_cl);

		if ((rnd == 1) && (gsets[POSITIVE].traces_num() == 0 || gsets[NEGATIVE].traces_num() == 0)) {
#ifdef __PRT
			if (gsets[POSITIVE].traces_num() == 0) 
				std::cout << RED << "\tZero Positive trace, execute program again." << NORMAL << std::endl;
			if (gsets[NEGATIVE].traces_num() == 0) 
				std::cout << RED << "\tZero Negative trace, execute program again." << NORMAL << std::endl;
#else
			std::cout << "+";
#endif
			if (++zero_times >= Nretry_init) {
				if (gsets[POSITIVE].traces_num() == 0) 
					std::cout << RED << "Can not get any positive trace. " << std::endl;
				else
					std::cout << RED << "Can not get any negative trace. " << std::endl;
				std::cout << " re-Run the system again OR modify your loop program.\n" << NORMAL;
				exit(-1);
			}
			if (maxv <= 10000000) {maxv+=base_maxv;}
			if (minv >= -10000000) {minv+=base_minv;}
			goto init_svm_i;
		}

#ifdef __PRT
		std::cout << "\t(" << step++ << ") prepare training data... ";
#else
		if (zero_times == 0) {
			std::cout << "]" << NORMAL;
		}
#endif
		if (svm_i->makeTrainingSet(gsets, pre_psize, pre_nsize) == 0) {
			if (++zero_times < Nretry_init)
				goto init_svm_i;
		}

#ifdef __PRT
		std::cout << "\n\t(" << step++ << ") start training... ";
#endif
#ifdef __DS_ENABLED
		std::cout << "[" << svm_i->problem.np << "+:" << svm_i->negative_size << "-]";
#endif
		int ret = svm_i->train();
		if (ret == -1) {
			std::cerr << RED << "[FAIL] ..... Can not divided by SVM_I." << std::endl << NORMAL;
			return -1;
		}
		//svm_i->cl.roundoff();
		std::cout << "|-->> " << YELLOW << *svm_i << std::endl << NORMAL;

		/*
		 *	check on its own training data.
		 *	There should be no prediction errors.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") checking training traces.";
#endif
		pass_rate = svm_i->checkTrainingSet();

#ifdef __PRT
		if (pass_rate == 1) 
			std::cout << GREEN << " [" << pass_rate * 100 << "%]" << NORMAL;
		else 
			std::cout << RED << " [" << pass_rate * 100 << "%]" << NORMAL;
#endif

		if (pass_rate < 1) {
			std::cerr << RED << "[FAIL] ..... Can not divided by SVM_I." << std::endl << NORMAL;
			rnd++;
			break;	
		}
#ifdef __PRT
		std::cout << GREEN << " [PASS]" << std::endl << NORMAL;
#endif

		/*
		 *	lastSimilar is used to store the convergence check return value for the last time.
		 *	We only admit convergence if the three consecutive round are converged.
		 *	This is to prevent in some round the points are too right to adjust the classifier.
		 */
#ifdef __PRT
		std::cout << "\t(" << step++ << ") check convergence:        ";
#endif
		if (svm_i->converged(pre_cl) == true) {
			if (lastSimilar == true) {
#ifdef __PRT
				std::cout << "[TT]  [SUCCESS] rounding off" << std::endl;
#endif
				converged = true;
				rnd++;
				break;
			}
#ifdef __PRT
			std::cout << "[FT]";
#endif
			lastSimilar = true;
		} else {
#ifdef __PRT
			std::cout << ((lastSimilar == true) ? "[T" : "[F") << "F] ";
#endif
			lastSimilar = false;
		}
#ifdef __PRT
		std::cout << "  [FAIL] neXt round " << std::endl;
#endif

		pre_cl = svm_i->cl;
		svm_i->cl.clear();
	} // end of SVM_I training procedure


	std::cout << "---------------------------------------------------\n";
	std::cout << "Finish running svm_i for " << rnd - 1 << " times." << std::endl;

	int ret = 0;
	if ((converged) && (rnd <= max_iteration)) {
		svm_i->cl.roundoff();
		svm_i->cl.simplify();
		std::cout << YELLOW << "  Hypothesis Invairant(Conjunctive): { ";
		std::cout << GREEN << svm_i->cl << YELLOW;
		std::cout << " }" << NORMAL << std::endl;
	}

	if ((pass_rate < 1) || (rnd > max_iteration)) {
		//std::cout << RED << "  Cannot divided by SVM_I perfectly.\n" << NORMAL;
		std::cout << pass_rate << "\t" << rnd << std::endl;
		ret = -1;
	}

	//std::cout << "return value = " << ret << std::endl;
	return ret;
}


std::string ConjunctiveLearner::invariant(int n) {
	svm_i->cl.roundoff();
	return svm_i->cl.toString();
}

int ConjunctiveLearner::save2file() {
	std::ofstream fout("../tmp/svm.ds");
	fout << svm_i->problem.np + svm_i->negative_size  << "\t" << svm_i->problem.np << "\t" << svm_i->negative_size << "\n";
	for (int i = 0; i < svm_i->problem.np; i++) {
		fout << 1;
		for (int j = 0; j < Nv; j++)
			fout << "\t" << j << ":" << (svm_i->problem.x[i][j]).value;
		fout << "\n";
	}
	for (int i = 0; i < svm_i->negative_size; i++) {
		fout << -1;
		for (int j = 0; j < Nv; j++)
			fout << "\t" << j << ":" << svm_i->negative_mapped_data[i][j];
		fout << "\n";
	}
	fout.close();
	std::cout << "save to file succeed. ../tmp/svm.ds\n";
	return 0;
}
