#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume( x < 100 && y < 100);
while( x < 100 && y < 100 )
{
recordi(x, y); x=x+1; y=y+1;
}
recordi(x, y);
iif_assert( x == 100 || y == 100);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test8.var", loopFunction, "loopFunction", "../tmp/interproc_test8.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test8.cnt", "../tmp/interproc_test8");
}
