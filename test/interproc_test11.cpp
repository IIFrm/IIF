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
 if (x < 0) x = x + 7; else x = x + 10;
if (y < 0) y = y - 10; else y = y + 3;
}
recordi(x, y);
iif_assert( x >= y && x <= y + 16);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/interproc_test11.var", loopFunction, "loopFunction", "../tmp/interproc_test11.ds");
context.addLearner("linear");
return context.learn("../tmp/interproc_test11.cnt", "../tmp/interproc_test11");
}
