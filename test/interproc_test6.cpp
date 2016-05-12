#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];

iif_assume( i < 10 && i > -10);
while( i * i < 100)
{
recordi(i);
 i = i + 1;
}
recordi(i);
iif_assert( i == 10);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test6.var", loopFunction, "loopFunction", "../tmp/interproc_test6.ds");
context.addLearner("poly");
return context.learn("../tmp/interproc_test6.cnt", "../tmp/interproc_test6");
}
