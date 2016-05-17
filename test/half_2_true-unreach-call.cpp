#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int k = _reserved_input_[1];
int n = _reserved_input_[2];

iif_assume(i==0 && k==n && n>=0);
while(i < n)
{
recordi(i, k, n);
k--; i=i+2;
}
recordi(i, k, n);
iif_assert( k==n-(n+1)/2);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/half_2_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/half_2_true-unreach-call.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/half_2_true-unreach-call.cnt", "../tmp/half_2_true-unreach-call");
}
