#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int x = _reserved_input_[0];
int y = _reserved_input_[1];

iif_assume(x==0 && y==50);
while(x<100)
{
recordi(x, y);
if(x<50) x++; else {x++; y++;}
}
recordi(x, y);
iif_assert(y==100);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/gj2007_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/gj2007_true-unreach-call.ds");
context.addLearner("conjunctive");
return context.learn("../tmp/gj2007_true-unreach-call.cnt", "../tmp/gj2007_true-unreach-call");
}
