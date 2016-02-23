/** @file classifier.h
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#ifndef _CLASSIFIER_
#define _CLASSIFIER_

#include "config.h"
#include "states.h"
#include "equation.h"
#include "connector.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class Classifier{
	protected:
		int max_size;
		int size;
		Equation* eqs;
		Connector* cts;

	public:
		Classifier(int maxsize = 16) {
			max_size = maxsize;
			eqs = new Equation[max_size];
			cts = new Connector[max_size];
			size = 0;
		}
		
		~Classifier() { 
			if (eqs) delete []eqs;
			if (cts) delete []cts;
		} 

		int getSize() const {
			return size;
		}

		int add(Equation* eq, Connector* ct) {
			if (size >= max_size)
				return -1;
			eqs[size] = *eq;
			cts[size] = *ct;
			return ++size;
		}

		int add(Equation* eq, int type = CONJUNCT) {
			if (size >= max_size)
				return -1;
			eqs[size] = *eq;
			cts[size].setType(type);
			return ++size;
		}
		
		std::string toString() {
			std::ostringstream stm;
			if (size <= 0) {
				stm << "";
				return stm.str();
			}
			stm << "(" << eqs[0] << ") ";
			for (int i = 1; i < size; i++) 
				stm << cts[i] << " (" << eqs[i] << ") ";
			return stm.str();
		}

	friend std::ostream& operator << (std::ostream& out, const Classifier& cs) {
		int size = cs.size;
		if (size <= 0) {
			out << "EMPTY classifier..\n";
			return out;
		}
		out << cs.eqs[0] << std::endl;
		for (int i = 1; i < size; i++) 
			out << cs.cts[i] << cs.eqs[i] << std::endl;
		return out;
	}
};


#endif
