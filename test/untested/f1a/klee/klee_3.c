#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<klee/klee.h>



int main (int argc, char** argv) {
	int x;
	int y;

	klee_make_symbolic(&x, sizeof(x), "x");
	klee_make_symbolic(&y, sizeof(y), "y");
//	klee_assume((1.99946831462933) * x +  (1.99946259758235) * y >= 0.999016667912656);
	klee_assume (x < 0);
	klee_assume (x + y > -1);
	klee_assert (y >= 0);
	return 0;
}

