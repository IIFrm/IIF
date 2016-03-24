#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int xa = a[0];
int ya = a[1];
int loop_times = a[2];

loop_times%=10; int branch=1;int x, y;
iif_assume(xa + 2 * ya >= 0);
while(loop_times-->=0)
{
recordi(xa, ya, loop_times); x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (branch) y = y + x;
		else y = y - x;
		branch = 1 - branch;

		xa = x - 2 * y;
		ya = 2 * x + y;
}
recordi(xa, ya, loop_times);iif_assert(xa + 2 * ya >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/ex1.var", loopFunction, "loopFunction", "../tmp/ex1.ds");
context.addLearner("linear", "../tmp/ex1.cnt");
return context.learn("../tmp/ex1");
}
