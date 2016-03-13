/** @file classifier.h
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#include "candidates.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

int Candidates::getSize() const {
	return size;
}

int Candidates::add(Equation* eq) {
	Equation* e = new Equation(*eq);
	cands.push_back(e);
	return ++size;
}

int Candidates::clear() {
	for (int i = 0; i < size; i++)
		delete cands[i];
	cands.clear();
	size = 0;
	return 0;
}

std::string Candidates::toString(int i) {
	if ((i >= size) || (i < 0))
		return NULL;
	return cands[i]->toString();
}

std::ostream& operator<< (std::ostream& out, const Candidates& cs) {
		int size = cs.size;
		if (size <= 0) {
			return out;
		}
		for (int i = 0; i < size; i++) 
			out << "candidate[" << i << "] :" << *cs.cands[i] << "\n"; 
		return out;
	}
