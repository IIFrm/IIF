#include "iif.h"
using namespace iif;

LearnerNode::LearnerNode() {
	learner = NULL;
	next = NULL;
}
LearnerNode::~LearnerNode() {
	if (learner != NULL)
		delete learner;
}

iifContext::iifContext (States* ss) {
	gsets = ss;
	first = NULL;
	last = NULL;
	variables = NULL;
	vnum = 0;
}

iifContext::iifContext(const char* vfilename, int (*func)(int*), const char* func_name, const char* pasttestcase, int timeout) {
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
	gsets[CNT_EMPL].label = 2;
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

iifContext::~iifContext() {
	// clean up heap memory
	LearnerNode* p = first;
	LearnerNode* pp;

	while (p != NULL) {
		pp = p->next;
		delete p;
		p = pp;
	}
	States* ss = &gsets[NEGATIVE];
	delete []ss;
	if (variables != NULL)
		delete []variables;
}


iifContext& iifContext::addLearner(const char* learnerName) {
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

int iifContext::learn(const char* invfilename) {
#ifdef linux
	// we only support timeout in LINUX system
	// Because don't know how to easily implement the same function in windows system...:( 
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(timeout);
#endif
	LearnerNode* p = first;
	char filename[65]; 
	while (p) {
		int n_cands = p->learner->learn();
		if (n_cands > 0) {
#ifdef _multi_candidates_
			std::cout << n_cands << std::endl;
			for (int i = 0; i < n_cands; i++) {
				sprintf(filename, "%s_%d.inv", (char*)invfilename, i);
				std::ofstream invFile(filename);
				invFile << p->learner->invariant(i);
				invFile.close();
			}
#else
			sprintf(filename, "%s.inv", (char*)invfilename);
			std::ofstream invFile(filename);
			invFile << p->learner->invariant(0);
			invFile.close();
#endif
			return 0;
		}
		else
			p = p->next;
	}
	return -1;
}
