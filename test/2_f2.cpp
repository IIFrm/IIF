#include "iif.h"

static int nondet() {
	return rand() % 4;
	return rand() % 20;
}

int f2(int* a) {
	int x = a[0];
	int y = a[1];
	int z;
	int w;

	iif_assume(3 * x > y - 1);
	while (nondet()) {
		recordi(x, y);
		if (nondet()) {x++; y = y+2;}
		else if (nondet()) {
			if (x >= 4) {x++; y = y+3; z = z+10; w = w+10;}
		}
		else if (x >= z && w >= y+1) {x = -x; y = -y; }

	}
	recordi(x, y);
	iif_assert(3 * x > y - 1);
	return 0;
}
