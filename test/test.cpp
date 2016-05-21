#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int xa = _reserved_input_[0];
int ya = _reserved_input_[1];

iif_assume(xa + ya > 0);
while(xa > 0)
{
recordi(xa, ya);
xa--; 
ya++;
}
recordi(xa, ya);
iif_assert(ya >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/test.var", loopFunction, "loopFunction", "../tmp/test.ds");
context.addLearner("linear");
return context.learn("../tmp/test.cnt", "../tmp/test");
}
