#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume( x == 0 && y == 0);
while(rand() % 8)
{
recordi(x, y);
 x++; y++;
}
recordi(x, y);
iif_assert( x == y && x >= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/fig1a_1.var", loopFunction, "loopFunction", "../tmp/fig1a_1.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/fig1a_1.cnt", "../tmp/fig1a_1");
}
