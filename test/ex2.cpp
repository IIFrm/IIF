#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int xa = _reserved_input_[0];
int ya = _reserved_input_[1];

int branch;int x, y;
branch = 1;
iif_assume(xa + 2 * ya >= 0);
while(rand() % 8)
{
recordi(xa, ya);
 x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (branch) y = y + x;
		else y = y - x;
		branch = 1 - branch;

		xa = x - 2 * y;
		ya = 2 * x + y;
}
recordi(xa, ya);
iif_assert(xa + 2 * ya >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/ex2.var", loopFunction, "loopFunction", "../tmp/ex2.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/ex2.cnt", "../tmp/ex2");
}
