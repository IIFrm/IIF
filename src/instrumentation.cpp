#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
//#include "header.h"
#include "instrumentation.h"
#include <assert.h>

bool _passP = false;
bool _passQ = false;
int assume_times = 0;
int assert_times = 0;
char lt[4][10] =  { "Negative", "Question", "Positive", "Bugtrace"};
char(*LabelTable)[10] = &lt[1];

double temp_states[max_states_in_one_trace][VARS];
int temp_index;


#include "color.h"
int add_state_int(int first ...)
{
	va_list ap;
	va_start(ap, first);
	temp_states[temp_index][0] = first;
	for (int i = 1; i < VARS; i++) {
		temp_states[temp_index][i] = va_arg(ap, int);
	}
	va_end(ap);

#ifdef __PRT_TRACE
	set_console_color(std::cout, BLUE);
	std::cout << "(" << temp_states[temp_index][0];
	for (int i = 1; i < VARS; i++) {
	    std::cout << "," << temp_states[temp_index][i];
	}
	std::cout << ")";
	unset_console_color(std::cout);
#endif
	temp_index++;
	if (temp_index >= max_states_in_one_trace) {
		set_console_color(std::cout, RED);
		std::cout << "\nToo many states (>" << max_states_in_one_trace << ") in one execution. Stop here.\n";
		unset_console_color(std::cout);
		exit(-1);
	}
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
	if (temp_index >= max_states_in_one_trace) {
		set_console_color(std::cout, RED);
		std::cout << "\nToo many states (>" << max_states_in_one_trace << ") in one execution. Stop here.\n";
		unset_console_color(std::cout);
		exit(-1);
	}
	return 0;
}


int before_loop()
{
	//std::cout << "---> before_loop";
	temp_index = 0;
	_passP = false;
	_passQ = false;
	assume_times = 0;
	assert_times = 0;
	//std::cout << "[done]";
	return 0;
}


int after_loop(States* gsets)
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
	set_console_color(std::cout, BLUE);
	std::cout << "TRACE: ";
	for (int i = 0; i < temp_index; i++) {
		std::cout << "(" << temp_states[i][0];
		for (int j = 1; j < VARS; j++)
			std::cout << "," << temp_states[i][j];
		std::cout << ")->";
	}
	std::cout << "END[" << label << "]" << std::endl;
	unset_console_color(std::cout);

	//TS[label].addLoopTrace(LT);
#endif
	gsets[label].add_states(temp_states, temp_index);
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
