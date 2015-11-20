#ifndef _IIF_H_
#define _IIF_H_

#include "config.h"
#include "instrumentation.h"
#include "iif_assert.h"
#include <iostream>

int nondet() {
	return rand() % 4;
	return rand() % 20;
}

int conj(int*);
#endif