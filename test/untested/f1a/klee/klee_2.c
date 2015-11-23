#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<klee/klee.h>


int main (int argc, char** argv) {
	int x;
	int y;

	klee_make_symbolic(&x, sizeof(x), "x");
	klee_make_symbolic(&y, sizeof(y), "y");
	klee_assume(x + y > -1);
	klee_assume (x >= 0);
	x--;
	y++;
	klee_assert(x + y > -1);
	return 0;
}

