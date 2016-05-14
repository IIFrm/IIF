#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int i = _reserved_input_[0];
int j = _reserved_input_[1];
int x = _reserved_input_[2];
int y = _reserved_input_[3];

iif_assume(i*i>=j*j && (x==y));
while(x <= 100)
{
recordi(i, j, x, y);
x=x+i*i; y=y+j*j;
}
recordi(i, j, x, y);
iif_assert(x>=y);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/WP1.var", loopFunction, "loopFunction", "../tmp/WP1.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/WP1.cnt", "../tmp/WP1");
}
