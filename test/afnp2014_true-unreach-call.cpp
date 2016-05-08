#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume(x == 1 && y == 0);
while( y < 1000)
{
recordi(x, y); x = x + y; y = y + 1;
}
recordi(x, y);
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
