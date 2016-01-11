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
#include "svm_i.h"
#include "color.h"
#include "solution.h"
#include "equation.h"
#include "states.h"
#include "base_learner.h"
#include "linear_learner.h"
#include "conjunctive_learner.h"
#include "disjunctive_learner.h"
//#include "learner_pipeline.h"
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
//#define conj target_program
extern int minv, maxv;
extern std::string* variables;
extern int vnum;


/*States* initSystem(int (*func)(int*), const char* func_name = "UserFunction", int timeout = 60);

  int destroySystem(States* gsets);
  */

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

			iifContext (const char* vfilename, int (*func)(int*), const char* func_name = "Unknown", int timeout = 60) 
			{
				std::ifstream vfile(vfilename);
				vfile >> vnum;
				variables = new std::string[vnum];
				for (int i = 0; i < vnum; i++)
					vfile >> variables[i];
				vfile.close();

				States* ss = new States[4];
				gsets = &ss[1];
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
				/*if (variables != NULL)
					delete []variables;
					*/
			}


			iifContext& addLearner(const char* learnerName) {
				BaseLearner* newLearner = NULL;
				if (strcmp(learnerName, "linear") == 0)
					newLearner = new LinearLearner(gsets);
				else if (strcmp(learnerName, "conjunctive") == 0)
					newLearner = new ConjunctiveLearner(gsets);
				else if (strcmp(learnerName, "disjunctive") == 0)
					newLearner = new DisjunctiveLearner(gsets);

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

			int learn() {
#ifdef linux
				if (signal(SIGALRM, sig_alrm) == SIG_ERR)
					exit(-1);
				alarm(timeout);
#endif
				LearnerNode* p = first;
				while (p) {
					if (p->learner->learn() == 0)
						return 0;
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
