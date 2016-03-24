#include "iif.h"
#include <iostream>
using namespace iif;

int loopFunction(int a[]) {
	int i = a[0];
	int j = a[1];
	int x = a[2];
	int y = a[3];

	x=0; y=0;
	iif_assume(i>=j);
	while(x <= 100)
	{
		recordi(i, j, x, y);x=x+i; y=y+j;
	}
	recordi(i, j, x, y);iif_assert(x>=y);
	return 0;
}

int main(int argc, char** argv)
{
	iifContext context("../tmp/WP1.var", loopFunction, "loopFunction", "../tmp/WP1.ds");
	context.addLearner("linear", "../tmp/WP1.cnt");
	return context.learn("../tmp/WP1");
}
