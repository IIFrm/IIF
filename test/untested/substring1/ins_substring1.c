#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include "../header.h"
using namespace std;

bool passP = false;
bool passQ = false;

int main (int argc, char** argv) {
	if (argc < 3) return -1;
	int i, j;
	int k;
	i = atoi(argv[1]);
//	k = atoi(argv[2]) % 101;
	k = atoi(argv[2]) % 101;
	j = 0;


	if ((i >= 0) && (i <= k) && (k >= 0) && (k <= 100)) 
		passP = true;


	while (i < k) {
		if (passP) 
			cout << "? 1 1:" << i  << " 2:" << j << endl;
		else
			cout << "? -1 1:" << i << " 2:" << j << endl;
		i++;
		j++;
	}

	if (j < 101)
		passQ = true;
	if (passP) { // pass Precondition, Q should be passed!
		if (passQ) cout << "+ 1 1:" << i << " 2:" << j << endl;
		else cout << "x 1 1:" << i << " 2:" << j << endl;	
	} else { 
		if (passQ) cout << "- -1 1:" << i << " 2:" << j << endl;
		else cout << "+ -1 1:" << i << " 2:" << j << endl;
	}

	return 0;
}

