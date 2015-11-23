#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include "../header.h"
using namespace std;

bool passP = false;
bool passQ = false;

int main(int argc, char** argv){
	int x, y, w, z;
	if (argc < 3) return -1;
	x = atoi(argv[1]);
	y = atoi(argv[2]);
	z=w=0;

	if ((x < 4) || (y > 2))
		passP = true;

	while (nondet()) {
		if (passP) 
			cout << "? 1 1:" << x  << " 2:" << y << endl;
		else
			cout << "? -1 1:" << x << " 2:" << y << endl;
		if (nondet())
		{x = x+1; y = y+100;}
		else if (nondet()){
			if (x >= 4)
			{x = x+1; y = y+1;}
		}
		else if (y > 10*w && z >= 100*x)
		{y = -y;}
		w = w+1; z = z+10;
		x = x; /* work around VC gen bug */

	}

	if ((x < 4) || (y > 2))
		passQ = true;
	if (passP) {
		if (passQ) cout << "+ 1 1:" << x << " 2:" << y << endl;
		else cout << "x 1 1:" << x << " 2:" << y << endl;
	} else {
		if (passQ) cout << "- -1 1:" << x << " 2:" << y << endl;
		else cout << "+ -1 1:" << x << " 2:" << y << endl;
	}

	return 0;
}
