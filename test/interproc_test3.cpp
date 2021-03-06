#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int j = _reserved_input_[1];
int k = _reserved_input_[2];

iif_assume(i<j && k>i-j);
while(i<j)
{
recordi(i, j, k);
k=k+1; i=i+1;
}
recordi(i, j, k);
iif_assert(k > 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test3.var", loopFunction, "loopFunction", "../tmp/interproc_test3.ds");
context.addLearner("linear");
return context.learn("../tmp/interproc_test3.cnt", "../tmp/interproc_test3");
}
