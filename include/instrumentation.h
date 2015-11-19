#ifndef _INSTRUMENTATION_H_
#define _INSTRUMENTATION_H_
//#include "header.h"
#include "config.h"
#include "states.h"

#include <stdarg.h>

extern States* gsets;

enum { NEGATIVE = -1, QUESTION, POSITIVE, COUNT_EXAMPLE };	/* trace_type */

int add_state_int(int first, ...);
int add_state_double(double first, ...);


#ifdef WIN32  
	//#if VARS == 1
	//	#define recordi(first) add_state_int(first)
	//	#define recordd(first) add_state_double(first)
	//#else
		#define recordi(first, ...) add_state_int(first, ##__VA_ARGS__)
		#define recordd(first, ...) add_state_double(first, ##__VA_ARGS__)
	//#endif
#endif

#ifdef linux
	#define recordi(first, args ...) add_state_int(first, ##args)
	#define recordd(first, args ...) add_state_double(first, ##args)
#endif

#ifdef __MACH__ 
	#define recordi(first, args ...) add_state_int(first, ##args)
	#define recordd(first, args ...) add_state_double(first, ##args)
#endif

// function lists

int m(int*);
int m_int(int*);
int m_double(double*);


int before_loop();
int after_loop();

#endif
