#include "iif.h"

int f2(int* a) {
	int x = a[0];
	int y = a[1];
	iif_assume((x + y >= 0) && (x - y >= 0));
	while (y > 0) {
		recordi(x, y);
		x++;
		y--;
	}

	recordi(x, y);
	iif_assume((x + y >= 0) && (x - y >= 0));
	return 0;
}

int main()
{return 0;}
