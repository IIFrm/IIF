#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];

iif_assume(x<10);
while(x<10 )
{
recordi(x); x=x+1;
}
recordi(x);
iif_assert(x==10);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/xeq10.var", loopFunction, "loopFunction", "../tmp/xeq10.ds");
context.addLearner("linear", "../tmp/xeq10.cnt");
context.addLearner("conjunctive", "../tmp/xeq10.cnt");
return context.learn("../tmp/xeq10");
}
