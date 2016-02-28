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
#include "instrumentation.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>


class BaseLearner{
	public:
		BaseLearner(States* gsets, int (*func)(int*)): gsets(gsets), func(func) {
			init_gsets();
		}

		virtual ~BaseLearner() {} 

/** @brief This function runs the target_program with the given input
 *
 *  @param  input defines input values which are used to call target_program 
 */
		void runTarget(Solution& input) {
			assert(func != NULL || "Func equals NULL, ERROR!\n");
			//assert(&input != NULL);
			beforeLoop();

			int a[VARS];
			for (int i = 0; i < VARS; i++)
				a[i] = static_cast<int>(input.getVal(i));
			/*int b[D];
			stateReverseMapping(a, b);*/
			//target_program
			func(a);

			afterLoop(gsets);
			if (gsets[COUNTER_EXAMPLE].traces_num() > 0) {
				setColor(std::cout, RED);
				std::cout << "}\nProgram BUG! Program have encountered a Counter-Example trace." << std::endl;
				std::cout << gsets[COUNTER_EXAMPLE] << std::endl;
				setColor(std::cout);
				exit(-1);
			}
		}

		virtual int learn() = 0;

		virtual int selectiveSampling(int randn, int exen, int type, void* params) = 0;

		virtual std::string invariant() = 0;

	protected:
		void init_gsets() {
			gsets[NEGATIVE].label = -1;
			gsets[QUESTION].label = 0;
			gsets[POSITIVE].label = 1;
			gsets[COUNTER_EXAMPLE].label = 2;
		}
		States* gsets;
		int (*func)(int*);
};


#endif
