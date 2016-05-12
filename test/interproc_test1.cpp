#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];

iif_assume(x<0);
while(x<10 )
{
recordi(x);
 x=x+1;
}
recordi(x);
iif_assert(x==10);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test1.var", loopFunction, "loopFunction", "../tmp/interproc_test1.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/interproc_test1.cnt", "../tmp/interproc_test1");
}
