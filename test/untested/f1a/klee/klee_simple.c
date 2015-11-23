#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <klee/klee.h>


int main (int argc, char** argv) {
	int x;
	int y;
	klee_make_symbolic(&x, sizeof(x), "x");
	klee_make_symbolic(&y, sizeof(y), "y");
	klee_assume(x +  y > 0);
	x--;
	y++;
//	klee_assert((1) * x +  (1) * y > 0);
	klee_assert((2) * x +  (2) * y > 0);
	return 0;
}

