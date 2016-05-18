#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];
int n = _reserved_input_[2];

iif_assume(x < 0 && y < 0 && n > 0);
while(x < 2 * n)
{
recordi(x, y, n);
if ( x < n ) { x = x + 1; } else { x = x + 1; y = y + 1; }
}
recordi(x, y, n);
iif_assert(y == n);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/test3.var", loopFunction, "loopFunction", "../tmp/test3.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/test3.cnt", "../tmp/test3");
}
