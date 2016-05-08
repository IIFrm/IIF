#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reverved_input_[]) {
int x = _reverved_input_[0];
int y = _reverved_input_[1];

iif_assume( x == y && x >= 0);
while( x > 0 || x < 0)
{
recordi(x, y);
 x--; y--;
}
recordi(x, y);
iif_assert( y >= 0 && y <= 0);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/fig1a_2.var", loopFunction, "loopFunction", "../tmp/fig1a_2.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/fig1a_2.cnt", "../tmp/fig1a_2");
}
