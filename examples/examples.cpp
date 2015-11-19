//#include "header.h"
#include "config.h"
#include "instrumentation.h"
#include "iif_assert.h"

#include <iostream>

//#define conj m
//#define ex1 m


int nondet() {
	return rand() % 4;
	return rand() % 20;
}


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
