#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

int x = -1;
int y = -1;
bool passP = false;
bool passQ = false;


int main (int argc, char** argv) {
	if (argc < 3) return -1;
	x = atoi(argv[1]);
	y = atoi(argv[2]);

	//Precondition: x + y >= 0
	//assume (x + y >= 0)
	if (x + y >= 0)
		passP = true;

	while (x >= 0) {
		if (passP) 
			cout << "? 1 1:" << x << " 2:" << y << endl;
		else
			cout << "? -1 1:" << x << " 2:" << y << endl;
		x--;
		y++;
	}

	// Postcondition: y > 0
	// assert (y > 0);
	if (y > 0)
		passQ = true;
	if (passP) { // pass Precondition, Q should be passed!
		if (passQ) cout << "+ 1 1:" << x << " 2:" << y << endl;
		else cout << "x 1 1:" << x << " 2:" << y << endl;	 //	cerr << "!BUG! input:" << atoi(argv[1]) << " " << atoi(argv[2]) << endl;
	} else { // not pass Precondition, if not pass Postcondition, all are negative samples.
		if (passQ) cout << "- -1 1:" << x << " 2:" << y << endl;
		else cout << "+ -1 1:" << x << " 2:" << y << endl;
	}

	return 0;
}

