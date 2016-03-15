#include "iif.h"
#include <iostream>

int pv2(int* a)
{
	int x = a[0];
	int y = x;
	iif_assume((x >= 0) && (x <= 50));
	while (true) {
		recordi(x);
		if (x <= 50) {
			y++;
		}
		else 
			y--;
		if (y < 0) {
			break;
		}
		x++;
	}
	recordi(x);
	iif_assert(x == 102);
	return 0;
}
