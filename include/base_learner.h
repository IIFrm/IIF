/** @file iif_learn.h
 *  @brief Provide base class for all iif learning algorithms.
 *
 *  This file contains the necessary function support for iif learning algorithms.
 *  The function run_target is the most important function in this class for now. 
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#ifndef _LEARNER_BASE_
#define _LEARNER_BASE_

#include "config.h"
#include "states.h"
#include "equation.h"
#include "classifier.h"
#include "instrumentation.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>


class BaseLearner{
	public:
		BaseLearner(States* gsets, int (*func)(int*)): gsets(gsets), func(func) {
			//init_gsets();
			cl = new Classifier();
			equ = new Equation();
		}

		virtual ~BaseLearner() {
			delete cl;
			delete equ;
		} 

/** @brief This function runs the target_program with the given input
 *
 *  @param  input defines input values which are used to call target_program 
 */
		int runTarget(Solution& input) {
			assert(func != NULL || "Func equals NULL, ERROR!\n");
			//assert(&input != NULL);
			beforeLoop();

			int a[Nv];
			for (int i = 0; i < Nv; i++)
				a[i] = static_cast<int>(input.getVal(i));
			//target_program
			//std::cout << "----> run the loop function.\n";
			func(a);
			//std::cout << "\t<---- run the loop function.\n";

			int ret = afterLoop(gsets);
			if (gsets[COUNTER_EXAMPLE].traces_num() > 0) {
				std::cout << RED << "}\nProgram BUG! Program have encountered a Counter-Example trace." << std::endl;
				std::cout << gsets[COUNTER_EXAMPLE] << WHITE << std::endl;
				exit(-2);
			}
			//std::cout << "\n<---- return from runTarget method.\n";
			return ret;
		}

		virtual int learn() = 0;

		virtual int selectiveSampling(int randn, int exen, int type, void* params) = 0;

		virtual std::string invariant() = 0;

	protected:
		/*void init_gsets() {
			gsets[NEGATIVE].label = -1;
			gsets[QUESTION].label = 0;
			gsets[POSITIVE].label = 1;
			gsets[COUNTER_EXAMPLE].label = 2;
		}*/

		States* gsets;
		int (*func)(int*);
		Classifier* cl;
		Equation* equ;
};


#endif
