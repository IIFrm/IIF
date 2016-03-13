/** @file classifier.h
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#ifndef _CANDIDATES_
#define _CANDIDATES_

#include "config.h"
#include "states.h"
#include "equation.h"
//#include "connector.h"
//#include "classifier.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class Equation;

class Candidates{
	protected:
		int size;
		//std::vector<Classifier*> cands;
		std::vector<Equation*> cands;

	public:
		Candidates() {
			size = 0;
		}
		
		~Candidates() { 
			cands.clear();
		} 

		int getSize() const;

		int add(Equation* eq);

		int clear();

		std::string toString(int i = 0);

	friend std::ostream& operator << (std::ostream& out, const Candidates& cs);
};


#endif
