#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int x = a[0];
int y = a[1];

iif_assume( y == x);
while( x < 1024)
{
recordi(x, y); x++; y++;
}
recordi(x, y);
iif_assert(x == y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/multivar_true-unreach-call1.var", loopFunction, "loopFunction", "../tmp/multivar_true-unreach-call1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/multivar_true-unreach-call1.cnt", "../tmp/multivar_true-unreach-call1");
}
