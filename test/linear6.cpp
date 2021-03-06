#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int j = _reserved_input_[1];
int r = _reserved_input_[2];

iif_assume(r > i + j);
while(i > 0)
{
recordi(i, j, r);
 i = i - 1; j = j + 1;
}
recordi(i, j, r);
iif_assert(r > i + j);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/linear6.var", loopFunction, "loopFunction", "../tmp/linear6.ds");
context.addLearner("linear");
return context.learn("../tmp/linear6.cnt", "../tmp/linear6");
}
