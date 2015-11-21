#include "config.h"
#include "iif.h"
#include "instrumentation.h"
#include <iostream>

extern int assume_times, assert_times;
int(*target_program)(int*) = NULL;
//= conj;
//int(*target_program)(int*) = f1;


int minv = -100, maxv = 100;

bool check_target_program(int (*func)(int*))
{
	int a[VARS];
	assume_times = 0;
	assert_times = 0;
	func(a);
	if (assume_times != 1)
		return false;
	if (assert_times != 1)
		return false;
	return true;
}


bool register_program(int (*func)(int*), char* func_name) 
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

