#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include "instrumentation.h"
#include <assert.h>

bool _passP = false;
bool _passQ = false;
int assume_times = 0;
int assert_times = 0;
char lt[4][10] =  { "Negative", "Question", "Positive", "Bugtrace"};
char(*LabelTable)[10] = &lt[1];

double program_states[max_states_in_one_trace][D];
double converted_states[max_states_in_one_trace][VARS];
int state_index;

double* stateMapping(double* src, double* dst) // src is of D dimension, while dst is of VARS dimension
{
	if (D == VARS) {
		for (int i = 0; i < D; i++) {
			dst[i] = src[i];
		}
		return dst;
	}
	switch (D) {
		case 1:
			// mapping rule: (x) --> (x, x^2)
			dst[0] = src[0];
			dst[1] = src[0] * src[0];
			break;
		case 2:
			// mapping rule: (x0, x1) --> (x0, x1, x0^2, x0*x1, x1^2)
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[0] * src[0];
			dst[3] = src[0] * src[1];
			dst[4] = src[1] * src[1];
			break;
		default:
			break;
	}
	return dst;
}

// src is of VARS dimension, while dst is of D dimension
double* stateReverseMapping(double* src, double* dst) {
	for (int i = 0; i < D; i++)
		dst[i] = src[i];
	return dst;
}
int* stateReverseMapping(int* src, int* dst) {
	for (int i = 0; i < D; i++)
		dst[i] = src[i];
	return dst;
}


#include "color.h"
int addStateInt(int first ...)
{
	va_list ap;
	va_start(ap, first);
	program_states[state_index][0] = first;
	for (int i = 1; i < D; i++) {
		program_states[state_index][i] = va_arg(ap, int);
	}
	va_end(ap);

#ifdef __PRT_TRACE
	setColor(std::cout, BLUE);
	std::cout << "(" << program_states[state_index][0];
	for (int i = 1; i < D; i++) {
	    std::cout << "," << program_states[state_index][i];
	}
	std::cout << ")";
	setColor(std::cout);
#endif
	stateMapping(program_states[state_index], converted_states[state_index]);
	state_index++;
	if (state_index >= max_states_in_one_trace) {
		setColor(std::cout, RED);
		std::cout << "\nToo many states (>" << max_states_in_one_trace << ") in one execution. Stop here.\n";
		setColor(std::cout);
		exit(-1);
	}
	return 0;
}

int addStateDouble(double first, ...)
{
	va_list ap;
	va_start(ap, first);
	program_states[state_index][0] = first;
	for (int i = 1; i < D; i++) {
		program_states[state_index][i] = va_arg(ap, double);
	}
	va_end(ap);
	stateMapping(program_states[state_index], converted_states[state_index]);
	state_index++;
	if (state_index >= max_states_in_one_trace) {
		setColor(std::cout, RED);
		std::cout << "\nToo many states (>" << max_states_in_one_trace << ") in one execution. Stop here.\n";
		setColor(std::cout);
		exit(-1);
	}
	return 0;
}


int beforeLoop()
{
	//std::cout << "---> before_loop";
	state_index = 0;
	_passP = false;
	_passQ = false;
	assume_times = 0;
	assert_times = 0;
	//std::cout << "[done]";
	return 0;
}


int afterLoop(States* gsets)
{
	int label = 0;
	assert(assume_times == 1);
	assert(assert_times == 1);
	if (_passP && _passQ) {
		label = 1;
	} else if (!_passP && !_passQ) {
		label = -1; 
	} else if (!_passP && _passQ) {
		label = 0; 
	} else if (_passP && !_passQ) {
		label = 2;
	}

#ifdef __PRT_TRACE
	setColor(std::cout, BLUE);
	std::cout << "TRACE: ";
	for (int i = 0; i < state_index; i++) {
		std::cout << "(" << program_states[i][0];
		for (int j = 1; j < D; j++)
			std::cout << "," << program_states[i][j];
		std::cout << ")->";
	}
	std::cout << "END[" << label << "]" << std::endl;
	setColor(std::cout);

	//TS[label].addLoopTrace(LT);
#endif
	gsets[label].addStates(converted_states, state_index);
	return 0;
}


int mDouble(double* p)
{
	int a[D];
	for (int i = 0; i < D; i++)
		a[i] = static_cast<int>(p[i]);
	return mInt(a);
}

int mInt(int* p) { return target_program(p); }
