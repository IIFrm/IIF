#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int j = _reserved_input_[0];
int k = _reserved_input_[1];
int n = _reserved_input_[2];

iif_assume( (j==0) && (k==n) && (n>0));
while(j<n && n>0)
{
recordi(j, k, n);
j++;k--;
}
recordi(j, k, n);
iif_assert( (k == 0));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/up_true-unreach-call_2.var", loopFunction, "loopFunction", "../tmp/up_true-unreach-call_2.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/up_true-unreach-call_2.cnt", "../tmp/up_true-unreach-call_2");
}
