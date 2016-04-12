#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume(x>0 && y<x*x/2 + x/2 && y>0);
while(y>0)
{
recordi(x, y);y=y-x;x--;
}
recordi(x, y);
iif_assert(x>=0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/lili.var", loopFunction, "loopFunction", "../tmp/lili.ds");
context.addLearner("conjunctive", "../tmp/lili.cnt");
return context.learn("../tmp/lili");
}
