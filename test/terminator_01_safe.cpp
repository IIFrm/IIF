#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];

iif_assume(x>=0);
while(x<100 && x>=0)
{
recordi(x);
x++;
}
recordi(x);
iif_assert(x>=100);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/terminator_01_safe.var", loopFunction, "loopFunction", "../tmp/terminator_01_safe.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/terminator_01_safe.cnt", "../tmp/terminator_01_safe");
}
