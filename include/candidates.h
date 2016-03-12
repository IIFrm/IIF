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
#include "connector.h"
#include "classifier.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class Candidates{
	protected:
		int size;
		std::vector<Classifier*> cands;

	public:
		Candidates(int maxsize = 16) {
			size = 0;
		}
		
		~Candidates() { 
			cands.clear();
		} 


		int getSize() const {
			return size;
		}

		int add(Classifier* cl) {
			cands.push_back(cl);
			return ++size;
		}

		int clear() {
			size = 0;
			return 0;
		}

		static int solver(const Classifier* cl, Solution& sol) {
			/*if ((cl == NULL) || (cl->size == 0)) 
				return Equation::linear_solver(NULL, sol);
			int select_equation = rand() % cl->size;
			return Equation::linear_solver(&(cl->eqs[select_equation]), sol);
			*/
			return 0;
		}


	friend std::ostream& operator << (std::ostream& out, const Candidates& cs) {
		int size = cs.size;
		if (size <= 0) {
			out << "EMPTY candidate..";
			return out;
		}
		out << " (" << cs.cands[0] << ") ";
		for (int i = 1; i < size; i++) 
			out << cs.cands[i] << "\n"; 
		return out;
	}
};


#endif
