#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int k = _reserved_input_[1];

iif_assume(i+k>-50 && i+k<50 && i==0);
while(i != 1000)
{
recordi(i, k);
 i=i+1; k=k-1;
}
recordi(i, k);
iif_assert(i+k>-50 && i+k<50);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/test2.var", loopFunction, "loopFunction", "../tmp/test2.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/test2.cnt", "../tmp/test2");
}
