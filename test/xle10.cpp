#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];

iif_assume(x>=0);
while((x>=0) && (x<10) )
{
recordi(x); x=x+1;
}
recordi(x);
iif_assert(x>=10);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/xle10.var", loopFunction, "loopFunction", "../tmp/xle10.ds");
context.addLearner("linear", "../tmp/xle10.cnt");
context.addLearner("conjunctive", "../tmp/xle10.cnt");
return context.learn("../tmp/xle10");
}