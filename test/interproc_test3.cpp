#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int i = a[0];
int j = a[1];
int k = a[2];

iif_assume(i<j&&k>0);
while(i<j)
{
recordi(i, j, k);k=k+1;i=i+1;
}
recordi(i, j, k);
iif_assert(k>j-i);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test3.var", loopFunction, "loopFunction", "../tmp/interproc_test3.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test3.cnt", "../tmp/interproc_test3");
}
