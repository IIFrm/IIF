#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
int i = a[0];
int k = a[1];
int n = a[2];

iif_assume( (i==0) && (k==0) && (n>0));
while(i < n)
{
recordi(i, k, n);i++;k++;
}
recordi(i, k, n);
iif_assert( (i == k) && (k == n));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/down_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/down_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/down_true-unreach-call.cnt", "../tmp/down_true-unreach-call");
}
