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

		int clear() {
			size = 0;
			return 0;
		}

		int factorization(Equation& eq) {
			std::cout << "Factorization Process >>>>\n";
			int mapping_type = eq.getMappingType();
			if (mapping_type == 1) {
				std::cout << "univariant linear function: " << eq << "\n";
				this->add(&eq);
				return 0;
			}

			if (mapping_type == 2) {
				if (VARS == 1) {
					// univariant quadratic function
					std::cout << "univariant quadratic function: " << eq << "\n";
					double A, B, C;
					A = eq.theta[1];
					if (A == 0) {
						eq.setMappingType(1);
						this->add(&eq);
						return 0;
					}
					B = eq.theta[0];
					C = eq.theta0;
					double delta = B * B - 4 * A * C;
					//std::cout << "A=" << A << " B=" << B << " C= " << C << " delta=" << delta << std::endl;
					if (delta < 0) return -1;
					double x1, x2;
					x1 = (-1 * B + sqrt(delta)) / (2 * A);
					x2 = (-1 * B - sqrt(delta)) / (2 * A);
					if (x1 > x2) std::swap(x1, x2);
					//std::cout << "x1=" << x1 << ", x2=" << x2 << "\n";
					if (A > 0) {
						this->add((new Equation(x1, -1.0))->roundoff());
						this->add((new Equation(-1 * x2, 1.0))->roundoff(), DISJUNCT);
					} else {
						this->add((new Equation(-1 * x1, 1.0))->roundoff());
						this->add((new Equation(x2, -1.0))->roundoff());
					}
					std::cout << *this << std::endl;
					return 0;
				}
			}

			return -1;
		}
		
		static int solver(const Classifier* cl, Solution& sol) {
			if ((cl == NULL) || (cl->size == 0)) 
				return Equation::linear_solver(NULL, sol);
			int select_equation = rand() % cl->size;
			return Equation::linear_solver(&(cl->eqs[select_equation]), sol);
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
			out << "EMPTY classifier..";
			return out;
		}
		out << " (" << cs.eqs[0] << ") ";
		for (int i = 1; i < size; i++) 
			out << cs.cts[i] << " (" << cs.eqs[i] << ") ";
		return out;
	}
};


#endif
