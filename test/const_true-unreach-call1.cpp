#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume( y < 1024);
while( y < 1024)
{
recordi(x, y); x = 0; y++;
}
recordi(x, y);
iif_assert( x == 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/const_true-unreach-call1.var", loopFunction, "loopFunction", "../tmp/const_true-unreach-call1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/const_true-unreach-call1.cnt", "../tmp/const_true-unreach-call1");
}
