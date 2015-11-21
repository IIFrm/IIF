//#include "header.h"
#include "iif.h"

int f3(int* a) {
	int x = a[0];
	int y = a[1];
	int z = a[2];
	iif_assume(x + y + z >= 0);
	while (z> 0) {
		recordi(x, y, z);
		x++;
		y++;
		z -= 2;
	}

	recordi(x, y, z);
	iif_assert(x + y >= 0);
	return 0;
}

int main()
{return 0;}
