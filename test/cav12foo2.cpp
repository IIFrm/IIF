#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume(x == y && x >= 0);
while(x > 0)
{
recordi(x, y);
x--; y--;
}
recordi(x, y);
iif_assert(y==0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/cav12foo2.var", loopFunction, "loopFunction", "../tmp/cav12foo2.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/cav12foo2.cnt", "../tmp/cav12foo2");
}
