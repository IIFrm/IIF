#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<klee/klee.h>


double x;
double y;

int loop()
{
	x--;
	y++;
	//	klee_assume(xa + 2 * ya >= 0);
}

int main (int argc, char** argv) {
	klee_make_symbolic(&x, sizeof(x), "x");
	klee_make_symbolic(&y, sizeof(y), "y");
	klee_assume(x +  y >= 0);
	klee_assume(0.9800000005 * x + 0.985000000002 * y >= -0.562);
	loop();
	klee_assert(0.985 * x + 0.985 * y >= -0.562);
//	klee_assert(xa + 2 * ya < 0);
	return 0;
}

