#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int x = _reverved_input_[0];
int y = _reverved_input_[1];

iif_assume(x == 0 && y == 0);
while(1)
{
recordi(x, y);
y = y + 1; x = x + y; if (rand()%5 == 0) break;
}
recordi(x, y);
iif_assert(x >= y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/esop13_1.var", loopFunction, "loopFunction", "../tmp/esop13_1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/esop13_1.cnt", "../tmp/esop13_1");
}
