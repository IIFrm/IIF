#include "iif.h"

int conj(int* a)
{
	int x;
	x = a[0];
	iif_assume((x > 0) && (x < 50));
	int flag = 1;
	while (nondet()) {
		recordi(x);
		x = x + flag;
		if (x >= 50) {
			x -= 2;
			flag = -1;
		}
		else if (x <= 0) {
			x += 2;
			flag = 1;
		}
	}
	recordi(x);
	iif_assert((x > 0) && (x < 50));
	return 0;
}

int(*target_program)(int*) = conj;