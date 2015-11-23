#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include "../header.h"
using namespace std;


int x;
int y;
bool passP = false;
bool passQ = false;

int main (int argc, char** argv) {
	srand(time(NULL));
	if (argc < 3) return -1;
	int xa = atoi(argv[1]);
	int ya = atoi(argv[2]);

	//Precondition: x + y >= 0
	//assume (x + y >= 0)
	if (xa + 2 * ya >= 0)
		passP = true;

	int loopnum1 = rand() % 20;
	while (loopnum1--) {
		if (passP) 
			cout << "? 1 1:" << xa  << " 2:" << ya << endl;
		else
			cout << "? -1 1:" << xa << " 2:" << ya << endl;
		int loopnum2 = rand() % 10;
		x = xa + 2*ya;
		y = -2*xa + ya;

		x++;
		if (loopnum2--) y = y+x;
		else y = y-x;

		xa = x - 2*y;
		ya = 2*x + y;
	}

	// Postcondition: y > 0
	// assert (y > 0);
	if (xa + 2 * ya >= 0)
		passQ = true;
	if (passP) { // pass Precondition, Q should be passed!
		if (passQ) cout << "+ 1 1:" << xa << " 2:" << ya << endl;
		else cout << "x 1 1:" << xa << " 2:" << ya << endl;	 //	cerr << "!BUG! input:" << atoi(argv[1]) << " " << atoi(argv[2]) << endl;
	} else { // not pass Precondition, if not pass Postcondition, all are negative samples.
		if (passQ) cout << "- -1 1:" << xa << " 2:" << ya << endl;
		else cout << "+ -1 1:" << xa << " 2:" << ya << endl;
	}

	return 0;
}

