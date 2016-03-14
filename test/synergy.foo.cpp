#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

y=0;
iif_assume(y>=0);
while(y>=0)
{
recordi(x, y);
y = y + x;
}
recordi(x, y);
iif_assert(false);
return 0;
}


int main(int argc, char** argv)
 {
iifContext context("../tmp/synergy.foo.var", loopFunction, "loopFunction");
context.addLearner("linear");
return context.learn("../tmp/synergy.foo");
}
