/** @file iif_learn.h
 *  @brief Provide base class for all iif learning algorithms.
 *
 *  This file contains the necessary function support for iif learning algorithms.
 *  The function run_target is the most important function in this base class for now. 
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#ifndef _LEARNER_BASE_
#define _LEARNER_BASE_

#include "config.h"
#include "states.h"
#include "polynomial.h"
#include "classifier.h"
#include "candidates.h"
#include "instrumentation.h"
#include "color.h"

#include <iostream>
#include <iomanip>
#include <float.h>
#include <string.h>
#include <assert.h>


class BaseLearner{
	public:
		BaseLearner(States* gsets, const char* cntempl_fname = NULL, int (*func)(int*) = target_program):
			gsets(gsets), func(func) {
				//cs = new Candidates();
				//cl = new Classifier();
				//poly = new Polynomial();
				if (cntempl_fname!= NULL) {
					//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
					std::ifstream fin(cntempl_fname);
					if (fin) {
						Solution s;
						while (fin >> s) {
							std::cout.setf(std::ios::fixed);
							std::cout << BLUE << BOLD << "Test on Last Counter Example: " << std::setprecision(0) 
								<<s << " from file " << cntempl_fname << " --> " << NORMAL << WHITE;
							std::cout.unsetf(std::ios::fixed);
							int ret = runTarget(s);
							printRunResult(ret);
							std::cout << std::endl << WHITE;
						}
						fin.close();
						//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
					}
				}
			}

		virtual ~BaseLearner() {
			//delete cl;
			//delete poly;
		} 

		virtual int save2file() = 0;
		/** @brief This function runs the target_program with the given input
		 *
		 *  @param  input defines input values which are used to call target_program 
		 */
		int runTarget(Solution& input) {
			assert(func != NULL || "Func equals NULL, ERROR!\n");
			beforeLoop();

			//< convert the given input with double type to the input with int type 
			int a[Nv];
			for (int i = 0; i < Nv; i++)
				a[i] = static_cast<int>(input[i]);
			//target_program
			//std::cout << "----> run the loop function.\n";
			func(a);
			//std::cout << "\t<---- run the loop function.\n";

			int ret = afterLoop(gsets);
			if (gsets[CNT_EMPL].traces_num() > 0) {
				std::cout << RED << BOLD << "x.\nBUG! Program encountered a Counter-Example trace." << std::endl;
				std::cout.setf(std::ios::fixed);
				std::cout << std::setprecision(0) <<gsets[CNT_EMPL] << WHITE << std::endl;
				std::cout.unsetf(std::ios::fixed);
				exit(-2);
			}
			return ret;
		}

		/** @brief This method is the entrance for the whole learning procedure.
		 *		   Child class should implement it based on learning algorithm.
		 */
		virtual int learn() = 0;

		/** @brief This method is used to generate new input and drive the testing process.
		 *		   This method is actually does several jobs, depend on parameters. 
		 *		   It is better to split it into several methods.
		 *
		 *		   You can find details of each parameters in child class.
		 */
		int selectiveSampling(int randn, int exen, int type, Classifier* cl) {
#ifdef __PRT
			std::cout << "{" << GREEN;
#endif

			if ((type != 0) && (exen > type))
				randn += exen - type;
			Solution input;
			int ret = 0;
			for (int i = 0; i < randn; i++) {
				Classifier::solver(NULL, input);
				ret = runTarget(input);
#ifdef __PRT
				std::cout << input;
				printRunResult(ret);
				std::cout << "|";
#endif
			}
#ifdef __PRT
			std::cout << BLUE;
#endif
			if (type == 0) {
				for (int i = 0; i < exen; i++) {
					Classifier::solver(cl, input);
					ret = runTarget(input);
#ifdef __PRT
					std::cout << "|" << input;
					printRunResult(ret);
#endif
				}
			}

#ifdef __PRT
			std::cout << WHITE << "}" << std::endl;
#endif
			return randn + exen;
		}

		/** @brief This method try to return a readable string which describe the generated invariant.
		 *		   The returned method is also required to be a valid expression which can be used in C program,
		 *		   because we will directly insert the string to the program to be verified.
		 */
		virtual std::string invariant(int n) = 0;

	protected:
		States* gsets;
		int (*func)(int*);
		//Candidates* cs;
		//Classifier* cl;
		//Polynomial* poly;
};


#endif
