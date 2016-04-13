#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume(x == y);
while(x < 999999)
{
recordi(x, y);x++;y++;
}
recordi(x, y);
iif_assert(x==y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/cav12foo1.var", loopFunction, "loopFunction", "../tmp/cav12foo1.ds");
context.addLearner("linear", "../tmp/cav12foo1.cnt");
return context.learn("../tmp/cav12foo1");
}
