#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume(x >= 1 && y >= 0);
while(y < 1000)
{
recordi(x, y);
x = x + y; y = y + 1;
}
recordi(x, y);
iif_assert(x >= y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/test4.var", loopFunction, "loopFunction", "../tmp/test4.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/test4.cnt", "../tmp/test4");
}
