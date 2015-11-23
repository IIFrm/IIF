#include<klee/klee.h>

int x;
int y;
int xa;
int ya;
int cond;

int innerloop()
{
	klee_assume(xa + 2 * ya >= 0);
	x = xa + 2*ya;
	y = -2*xa + ya;

	x++;
	if (cond) y	= y+x;
	else y = y-x;

	xa = x - 2*y;
	ya = 2*x + y;
	klee_assert(xa + 2 * ya >= 0);
}

int main () {
	klee_make_symbolic(&cond, sizeof(cond), "cond");
	klee_make_symbolic(&x, sizeof(x), "x");
	klee_make_symbolic(&y, sizeof(y), "y");
	klee_make_symbolic(&xa, sizeof(xa), "xa");
	klee_make_symbolic(&ya, sizeof(ya), "ya");
	innerloop();
	return 0;
}

