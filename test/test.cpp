#include "iif.h"
#include <iostream>

int loopFunction(int a[]) {
int xa = a[0];
int ya = a[1];


iif_assume(xa + ya > 0);
while(xa > 0)
{
recordi(xa, ya);
xa--; 
ya++;
}
recordi(xa, ya);
iif_assert(ya > 0);
return 0;
}

int main(int argc, char** argv)
 {
LearnerPipeline lp(loopFunction, "loopFunction");
lp.addLearner("linear").addLearner("conjunctive");
return lp.learn();
}
