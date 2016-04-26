#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume( x<y);
while( x<y)
{
recordi(x, y); if (x % 2 == 0) x=x+100; else x = x + 50;
}
recordi(x, y);
iif_assert( x >= y && x <= y + 99);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test5.var", loopFunction, "loopFunction", "../tmp/interproc_test5.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test5.cnt", "../tmp/interproc_test5");
}
