#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int j = _reserved_input_[1];

iif_assume(i*i < j*j);
while(i*i < j*j)
{
recordi(i, j);
 if (j > 0) { j = j - 1; } else { j = j + 1; }
}
recordi(i, j);
iif_assert(i*i == j*j);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test7.var", loopFunction, "loopFunction", "../tmp/interproc_test7.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test7.cnt", "../tmp/interproc_test7");
}
