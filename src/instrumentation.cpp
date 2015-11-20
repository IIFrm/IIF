/*************************************************************************
  > File Name: iif.cpp
  > Author: Li Jiaying
  > Mail: lijiaying1989@gmail.com 
  > Created Time: Wed Oct 28 01:47:17 2015
 ************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
//#include "header.h"
#include "instrumentation.h"

bool _passP = false;
bool _passQ = false;
char lt[4][10] =  { "Negative", "Question", "Positive", "Bugtrace"};
char(*LabelTable)[10] = &lt[1];

double temp_states[256][VARS];
int temp_index;


int add_state_int(int first ...)
{
	va_list ap;
	va_start(ap, first);
	temp_states[temp_index][0] = first;
	for (int i = 1; i < VARS; i++) {
		temp_states[temp_index][i] = va_arg(ap, int);
	}
	va_end(ap);
	temp_index++;
	return 0;
}

int add_state_double(double first, ...)
{
	va_list ap;
	va_start(ap, first);
	temp_states[temp_index][0] = first;
	for (int i = 1; i < VARS; i++) {
		temp_states[temp_index][i] = va_arg(ap, double);
	}
	va_end(ap);
	temp_index++;
	return 0;
}


int before_loop()
{
	//std::cout << "---> before_loop";
	temp_index = 0;
	_passP = false;
	_passQ = false;
	//std::cout << "[done]";
	return 0;
}


int after_loop()
{
	int label = 0;
	if (_passP && _passQ) {
		label = 1;
	} else if (!_passP && !_passQ) {
		label = -1; 
	} else if (!_passP && _passQ) {
		label = 0; 
	} else if (_passP && !_passQ) {
		label = 2;
	}

	/*
	std::cout << "TRACE: ";
	for (int i = 0; i < temp_index; i++) {
		std::cout << "(" << temp_states[i][0];
		for (int j = 1; j < VARS; j++)
			std::cout << "," << temp_states[i][j];
		std::cout << ")->";
	}
	std::cout << "END[" << label << "]" << std::endl;
	*/

	gsets[label].add_states(temp_states, temp_index);
	//TS[label].addLoopTrace(LT);
	return 0;
}


int m_double(double* p)
{
	int a[VARS];
	for (int i = 0; i < VARS; i++)
		a[i] = static_cast<int>(p[i]);
	return m_int(a);
}

int m_int(int* p) { return target_program(p); }
