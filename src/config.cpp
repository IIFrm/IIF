#include "config.h"
#include "iif.h"
#include "instrumentation.h"
#include <iostream>
#include <stdlib.h>

extern int assume_times, assert_times;
int(*target_program)(int*) = NULL;

int minv = -100, maxv = 100;
std::string* variables;
int vnum;

bool check_target_program(int (*func)(int*))
{
    Solution sol;
    Equation::linear_solver(NULL, sol);
	int a[VARS];
	for (int i = 0; i < VARS; i++)
	    a[i] = sol.getVal(i);
	assume_times = 0;
	assert_times = 0;
	func(a);
	if (assume_times != 1)
		return false;
	if (assert_times != 1)
		return false;
	return true;
}


bool register_program(int (*func)(int*), const char* func_name) 
{
	if (check_target_program(func) == false) {
		if (func_name == NULL) {
			std::cout << "The target is not a valid program to test.\n";
		} else {
			std::cout << func_name << " is not a valid program to test.\n";
		}
		return false;
	}
	target_program = func;
	return true;
}

/*
void sig_alrm(int signo) {
	std::cout << "\nTIMEOUT!\n";
	exit(-1);
}


States* initSystem(int (*func)(int*), const char* func_name, int timeout) 
{
	States* ss = new States[4];
	srand(time(NULL)); // initialize seed for rand() function

	register_program(func, func_name);
#ifdef linux
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		exit(-1);
	alarm(timeout);
#endif
	
	return &ss[1];
}


int destroySystem(States* gsets)
{
	States* ss = &gsets[-1];
	delete []ss;
	return 0;
}
*/
