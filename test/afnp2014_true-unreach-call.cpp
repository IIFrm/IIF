#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];
int z = _reserved_input_[2];

iif_assume(x == 1 && y == 0);
while( y < 1000 && z <= 20)
{
recordi(x, y, z);
 x = x + y; y = y + 1; z = z + 1;
}
recordi(x, y, z);
iif_assert(x >= y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/afnp2014_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/afnp2014_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/afnp2014_true-unreach-call.cnt", "../tmp/afnp2014_true-unreach-call");
}
