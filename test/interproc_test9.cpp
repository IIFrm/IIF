#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int i = a[0];
int j = a[1];

iif_assume(i*i < j);
while(i*i < j)
{
recordi(i, j); i = i + 1;
}
recordi(i, j);
iif_assert((i - 1) * (i - 1) < j && j <= i * i);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test9.var", loopFunction, "loopFunction", "../tmp/interproc_test9.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test9.cnt", "../tmp/interproc_test9");
}
