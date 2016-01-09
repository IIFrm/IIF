#ifndef _LINEAR_PIPELINE_
#define _LINEAR_PIPELINE_
#include "config.h"
#include "iif.h"
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#ifdef linux
#include <sys/time.h>
#include <unistd.h>
#endif

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


class LearnerPipeline {
	private:
		static void sig_alrm(int signo) {
			std::cout << "\nTIMEOUT!\n";
			exit(-1);
		}

	public:
		LearnerPipeline(States* ss) {
			gsets = ss;
			first = NULL;
			last = NULL;
		}

		LearnerPipeline(int (*func)(int*), const char* func_name = "Unknown", int timeout = 60) 
		{
			srand(time(NULL)); // initialize seed for rand() function

			States* ss = new States[4];
			gsets = &ss[1];
			first = NULL;
			last = NULL;
			register_program(func, func_name);

#ifdef linux
			if (signal(SIGALRM, sig_alrm) == SIG_ERR)
				exit(-1);
			alarm(timeout);
#endif
		}

		~LearnerPipeline() {
			LearnerNode* p = first;
			LearnerNode* pp;
			while (p != NULL) {
				pp = p->next;
				delete p;
				p = pp;
			}
			States* ss = &gsets[-1];
			delete []ss;
		}

		LearnerPipeline& addLearner(const char* learnerName) {
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
};

#endif
