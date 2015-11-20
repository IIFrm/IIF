//#include "header.h"
#include "iif.h"


int f1(int* a){
	int x = a[0];
	int y = a[1];
	iif_assume (x + y > 0);
	while (y > 0) {
		recordi(x, y);
		x++;
		y--;
	}

	recordi(x, y);
	iif_assert (x > 0);
	return 0;
}
