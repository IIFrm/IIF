#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume( x<y);
while( x<y)
{
recordi(x, y);
 x=x+100;
}
recordi(x, y);
iif_assert( x >= y && x <= y + 99);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test5.var", loopFunction, "loopFunction", "../tmp/interproc_test5.ds");
context.addLearner("linear");
return context.learn("../tmp/interproc_test5.cnt", "../tmp/interproc_test5");
}
