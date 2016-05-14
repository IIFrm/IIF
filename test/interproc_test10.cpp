#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int j = _reserved_input_[1];

if(i < 0) i = -i; i = i + 1;
iif_assume((i+1)*(i+1) < j && i >= 1);
while((i+1)*(i+1) < j)
{
recordi(i, j);
 i = i + 1;
}
recordi(i, j);
iif_assert(i*i< j && j <= (i+1) * (i+1));
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test10.var", loopFunction, "loopFunction", "../tmp/interproc_test10.ds");
context.addLearner("poly");
return context.learn("../tmp/interproc_test10.cnt", "../tmp/interproc_test10");
}
