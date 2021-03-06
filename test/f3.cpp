#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];
int z = _reserved_input_[2];

iif_assume(x + y + z >= 0);
while(z > 0)
{
recordi(x, y, z);
x++; 
y++;
z-=2;
}
recordi(x, y, z);
iif_assert(x + y >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/f3.var", loopFunction, "loopFunction", "../tmp/f3.ds");
context.addLearner("linear");
return context.learn("../tmp/f3.cnt", "../tmp/f3");
}
