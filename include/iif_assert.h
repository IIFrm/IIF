#ifndef _IIF_ASSERT_H_
#define _IIF_ASSERT_H_

extern bool _passP;
extern bool _passQ;

//#undef assume
//#undef assert
extern int assume_times, assert_times;

#define iif_assume(expr) do { \
	_passP = (expr)? true : false;\
	assume_times++;\
} while(0)

#define iif_assert(expr) do { \
	_passQ = (expr)? true : false;\
	assert_times++;\
} while(0)

#endif
