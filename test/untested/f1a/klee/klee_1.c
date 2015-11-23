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
	klee_assume(x < 10000);
	klee_assume(y < 10000);
//	klee_assume(x >= -100000);
//	klee_assume(y >= -100000);
//	klee_assume(y <= 100000);
//	klee_assume(x <= 100000);
//	klee_assert((1.99946831462933) * x +  (1.99946259758235) * y >= 0.999016667912656);
//	klee_assert((2) * x +  (2) * y >= 1);
//	klee_assert((0.4) * x +  (0.4) * y >= -0.2);
	klee_assert(x +  y > -1);
	return 0;
}

