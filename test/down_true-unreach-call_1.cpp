#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int k = _reserved_input_[1];
int n = _reserved_input_[2];

iif_assume( (i==0) && (k==0) && (n>0));
while(i < n)
{
recordi(i, k, n);
i++;k++;
}
recordi(i, k, n);
iif_assert( (i == k) && (k == n));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/down_true-unreach-call_1.var", loopFunction, "loopFunction", "../tmp/down_true-unreach-call_1.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/down_true-unreach-call_1.cnt", "../tmp/down_true-unreach-call_1");
}
