/** @file iif.h
 *  @brief Contains all the files that needed to be included by a new test.
 *  
 *  By include this file, it should resolve all the reference errors to the framework.
 *
 *  @author Li Jiaying
 *  @bug No found bugs
 */
#ifndef _IIF_H_
#define _IIF_H_

#include "config.h"
#include "instrumentation.h"
#include "ml_algo.h"
#include "svm.h"
//#include "svm_i.h"
#include "color.h"
#include "solution.h"
#include "equation.h"
#include "connector.h"
#include "classifier.h"
#include "states.h"
#include "base_learner.h"
#include "linear_learner.h"
//#include "poly_learner.h"
//#include "rbf_learner.h"
//#include "conjunctive_learner.h"
//#include "disjunctive_learner.h"
#include "iif_assert.h"

#include <iostream>
#include <float.h>
#include <string>
#include <cassert>
#include <iostream>
#include <fstream>

#include <cstdlib>
#include <signal.h>
#ifdef linux
#include <sys/time.h>
#include <unistd.h>
#endif

extern int minv, maxv;
extern std::string* variables;
extern int vnum;


namespace iif{
	class LearnerNode {
		public:
			LearnerNode() {
				learner = NULL;
				next = NULL;
			}
			~LearnerNode() {
				if (learner != NULL)
					delete learner;
			}
			BaseLearner* learner;
			LearnerNode* next;
	};


	class iifContext {
		private:
			static void sig_alrm(int signo) {
				std::cout << "\nTIMEOUT!\n";
				exit(-1);
			}

		public:
			iifContext (States* ss) {
				gsets = ss;
				first = NULL;
				last = NULL;
				variables = NULL;
				vnum = 0;
			}

			iifContext (const char* vfilename, int (*func)(int*), const char* func_name = "Unknown", const char* pasttestcase = NULL, int timeout = 1800) {
				std::ifstream vfile(vfilename);
				vfile >> vnum;
				variables = new std::string[Cv0to4];
				variables[0] = '1';
				for (int i = 1; i <= Nv; i++)
					vfile >> variables[i];
				vfile.close();
				int index = Nv+1;
				for (int i = 1; i <= Nv; i++) {
					for (int j = i; j <= Nv; j++) {
						variables[index++] = "(" + variables[i] + "*" + variables[j] + ")";
					}
				}
				for (int i = 1; i <= Nv; i++) {
					for (int j = i; j <= Nv; j++) {
						for (int k = j; k <= Nv; k++) {
							variables[index++] = "(" + variables[i] + "*" + variables[j] + "*" + variables[k] + ")";
						}
					}
				}
				for (int i = 1; i <= Nv; i++) {
					for (int j = i; j <= Nv; j++) {
						for (int k = j; k <= Nv; k++) {
							for (int l = k; l <= Nv; l++) {
								variables[index++] = "(" + variables[i] + "*" + variables[j] + "*" + variables[k] + "*" + variables[l] + ")";
							}
						}
					}
				}

				States* ss = new States[4];
				gsets = &ss[1];
				gsets[NEGATIVE].label = -1;
				gsets[QUESTION].label = 0;
				gsets[POSITIVE].label = 1;
				gsets[COUNTER_EXAMPLE].label = 2;
				if (pasttestcase != NULL) {
					std::ifstream fin(pasttestcase);
					int l, pn, nn;
					fin >> l >> pn >> nn;
					gsets[POSITIVE].initFromFile(pn, fin);
					gsets[NEGATIVE].initFromFile(nn, fin);
					fin.close();
				}
				first = NULL;
				last = NULL;
				register_program(func, func_name);
				this->timeout = timeout;
				srand(time(NULL)); // initialize seed for rand() function
			}

			~iifContext() {
				LearnerNode* p = first;
				LearnerNode* pp;
				while (p != NULL) {
					pp = p->next;
					delete p;
					p = pp;
				}
				States* ss = &gsets[-1];
				delete []ss;
				if (variables != NULL)
					delete []variables;
			}


			iifContext& addLearner(const char* learnerName) {
				BaseLearner* newLearner = NULL;
				if (strcmp(learnerName, "linear") == 0)
					newLearner = new LinearLearner(gsets);
				/* 
				else if (strcmp(learnerName, "poly") == 0)
					newLearner = new PolyLearner(gsets);
				else if (strcmp(learnerName, "rbf") == 0)
					newLearner = new RbfLearner(gsets);
				else if (strcmp(learnerName, "conjunctive") == 0)
					newLearner = new ConjunctiveLearner(gsets);
				else if (strcmp(learnerName, "disjunctive") == 0)
					newLearner = new DisjunctiveLearner(gsets);
					*/

				if (last == NULL) {
					last = new LearnerNode();
					first = last;
				} else {
					last->next = new LearnerNode();
					last = last->next;
				}
				last->learner = newLearner;
				return *this;
			}

			int learn(const char* invfilename = ".inv") {
#ifdef linux
				if (signal(SIGALRM, sig_alrm) == SIG_ERR)
					exit(-1);
				alarm(timeout);
#endif
				LearnerNode* p = first;
				while (p) {
					if (p->learner->learn() == 0) {
						std::ofstream invFile(invfilename);
						invFile << p->learner->invariant();
						return 0;
					}
					else
						p = p->next;
				}
				return -1;
			}

		private:
			States* gsets;
			LearnerNode* first;
			LearnerNode* last; 
			int timeout;
	};
}
#endif
