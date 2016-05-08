#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int xa = _reverved_input_[0];
int ya = _reverved_input_[1];

int x, y;
iif_assume(xa + 2 * ya >= 0);
while(rand() % 2)
{
recordi(xa, ya);
 x = xa + 2 * ya;
		y = -2 * xa + ya;

		x++;
		if (rand()%2) y = y + x;
		else y = y - x;

		xa = x - 2 * y;
		ya = 2 * x + y;
}
recordi(xa, ya);
iif_assert(xa + 2 * ya >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/ex1.var", loopFunction, "loopFunction", "../tmp/ex1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/ex1.cnt", "../tmp/ex1");
}
