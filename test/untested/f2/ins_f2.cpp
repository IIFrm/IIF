#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include "../header.h"
using namespace std;

bool passP = false;
bool passQ = false;

int main (int argc, char** argv) {
	srand(time(NULL));
	int x, y, z, w;
	if (argc < 3) return -1;
	x = atoi(argv[1]);
	y = atoi(argv[2]);
	z=w=0;

	if (3 * x > y - 1)
		passP = true;

	while (nondet() ) {
		if (passP) 
			cout << "? 1 1:" << x  << " 2:" << y << endl;
		else
			cout << "? -1 1:" << x << " 2:" << y << endl;
		if (nondet()) {x++; y = y+2;}
		else if (nondet()) {
			if (x >= 4) {x++; y = y+3; z = z+10; w = w+10;}
		}
		else if (x >= z && w >= y+1) {x = -x; y = -y; }
		x = x;  /* this works around a VC gen bug */
	}

	if (3 * x > y - 1)
		passQ = true;
	if (passP) {
		if (passQ) cout << "+ 1 1:" << x << " 2:" << y << endl;
		else cout << "x 1 1:" << x << " 2:" << y << endl;
	} else {
		if (passQ) cout << "- -1 1:" << x << " 2:" << y << endl;
		else cout << "+ -1 1:" << x << " 2:" << y << endl;
	}

	return 0;
ERROR:
	return -1;
}
