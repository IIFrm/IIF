/*************************************************************************
  > File Name: main.cpp
  > Author: Li Jiaying
  > Mail: lijiaying1989@gmail.com 
  > Created Time: Wed Oct 28 01:47:17 2015
 ************************************************************************/
//#include "header.h"
#include "config.h"
#include "ml_algo.h"
#include "svm.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"

#include "equation.h"
#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

int minv = -100, maxv = 100;
void print_null(const char *s) {}

States global_states_sets[4];
States* gsets = &global_states_sets[1];



int main(int argc, char** argv)
{
	//char RET_TRANS[20][5] = {"False", "True", "Could Not Answer", "Timeout", "Unhandled Error"};
	Equation e1(-3.4, 1.44524345);
	Equation e2(-2.0, 1.5626422);
	std::cout << "e1: " << e1 << std::endl;
	std::cout << "e2: " << e2 << std::endl;
	int ret = e1.imply(e2);
	/*
	std::cout << "[" << e1 << "]\t==>\t[" << e2 << "]? ";
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	*/
	
	ret = e2.imply(e1);
	/*std::cout << "[" << e2 << "]\t==>\t[" << e1 << "]? ";
	switch(ret) {
		case 0: std::cout << "False\n"; break;
		case 1: std::cout << "True\n"; break;
		case 2: std::cout << "Could not answer query\n"; break;
		case 3: std::cout << "Timeout.\n"; break;
		default: std::cout << "Unhandled error\n";
	}
	*/
	return 0;
}

