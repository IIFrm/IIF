#ifndef _IIF_ASSERT_H_
#define _IIF_ASSERT_H_

extern bool _passP;
extern bool _passQ;

//#undef assume
//#undef assert

#define iif_assume(expr) do { \
	_passP = (expr)? true : false;\
} while(0)

#define iif_assert(expr) do { \
	_passQ = (expr)? true : false;\
} while(0)

#endif
