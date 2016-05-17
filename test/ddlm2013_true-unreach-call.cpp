#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int _reserved_input_[]) {
int a = _reserved_input_[0];
int b = _reserved_input_[1];
int i = _reserved_input_[2];
int j = _reserved_input_[3];

iif_assume( a==0 && b==0 && i==0 && j==1);
while(rand() % 8)
{
recordi(a, b, i, j);
a++; b+=(j-i); i+=2; if (i%2==0) j+=2; else j++;
}
recordi(a, b, i, j);
iif_assert(a==b);
return 0;
}

int main(int argc, char** argv)
 {
iifContext context("../tmp/ddlm2013_true-unreach-call.var", loopFunction, "loopFunction", "../tmp/ddlm2013_true-unreach-call.ds");
context.addLearner("linear");
context.addLearner("poly");
context.addLearner("conjunctive");
return context.learn("../tmp/ddlm2013_true-unreach-call.cnt", "../tmp/ddlm2013_true-unreach-call");
}
