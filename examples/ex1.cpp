#include "iif.h"

int ex1(int* a) {
	int x;
	int y;
	int xa = a[0];
	int ya = a[1];

	iif_assume(xa + 2 * ya >= 0);
	while (nondet()) {
		recordi(xa, ya);
		x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (nondet()) y = y + x;
		else y = y - x;

		xa = x - 2 * y;
		ya = 2 * x + y;
	}
	recordi(xa, ya);
	iif_assert(xa + 2 * ya >= 0);
	return 0;
}