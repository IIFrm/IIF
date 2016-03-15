#include "iif.h"
#include <iostream>

static int nondet() {
	return rand() % 4;
	return rand() % 20;
}

int conj(int* a)
{
	int x;
	x = a[0];
	int loop_times = a[1] % 10;
	iif_assume((x >= 0) && (x <= 50));
	while (loop_times-- > 0) {
		recordi(x, loop_times);
		if (x <= 0) {
			x ++;
		} else {
			x--;
		}
	}
	recordi(x, loop_times);
	iif_assert((x >= 0) && (x <= 50));
	return 0;
}
