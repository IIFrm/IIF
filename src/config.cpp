#include "config.h"
#include "iif.h"
#include "instrumentation.h"
#include <iostream>
#include <stdlib.h>

extern int assume_times, assert_times;
int(*target_program)(int*) = NULL;

int minv = -200, maxv = 200;
std::string* variables;
VariablePowerArray* vparray;
int vnum;

bool check_target_program(int (*func)(int*))
{
    Solution sol;
    Polynomial::linear_solver(NULL, sol);
	int a[Nv];
	for (int i = 0; i < Nv; i++)
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
