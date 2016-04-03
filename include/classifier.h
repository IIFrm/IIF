/** @file classifier.h
 *
 *  @author Li Jiaying
 *  @bug no known bugs found.
 */
#ifndef _CLASSIFIER_
#define _CLASSIFIER_

#include "config.h"
#include "states.h"
#include "polynomial.h"
#include "connector.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>

class Polynomial;

class Classifier{
	protected:
		int max_size;
		int size;
		Polynomial* eqs;
		Connector* cts;

	public:
		Classifier(int maxsize = 16) {
			max_size = maxsize;
			eqs = new Polynomial[max_size];
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

		int add(Polynomial* eq, Connector* ct) {
			if (size >= max_size)
				return -1;
			eqs[size] = *eq;
			cts[size] = *ct;
			return ++size;
		}

		int add(Polynomial* eq, int type = CONJUNCT) {
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

		bool factor(Polynomial& eq) {
			//std::cout << "\tFactorization Process >>>>\n";
			std::cout << GREEN << eq << WHITE <<std::endl;
			int etimes = eq.getEtimes();
			if (etimes == 1) {
				//std::cout << "univariant linear function: " << eq << "\n";
				this->add(&eq);
				return true;
			}

			if (Nv == 1) {
				if (etimes == 2) {
					// univariant quadratic function
					//std::cout << "univariant quadratic function: " << eq << "\n";
					double A, B, C;
					A = eq.getTheta(2);
					B = eq.getTheta(1);
					C = eq.getTheta(0);
					if (A == 0) {
						eq.setEtimes(1);
						this->add(&eq);
						return true;
					}
					double delta = B * B - 4 * A * C;
					//std::cout << "A=" << A << " B=" << B << " C= " << C << " delta=" << delta << std::endl;
					if (delta < 0) {
						std::cout << RED << "Delta shouldnot be less than 0.\n";
						return false;
					}
					double x1, x2;
					x1 = (-1 * B + sqrt(delta)) / (2 * A);
					x2 = (-1 * B - sqrt(delta)) / (2 * A);
					if (x1 > x2) std::swap(x1, x2);
					//std::cout << "x1=" << x1 << ", x2=" << x2 << "\n";
					if (A > 0) {
						this->add((new Polynomial(x1, -1.0))->roundoff());
						this->add((new Polynomial(-1 * x2, 1.0))->roundoff(), DISJUNCT);
					} else {
						this->add((new Polynomial(-1 * x1, 1.0))->roundoff());
						this->add((new Polynomial(x2, -1.0))->roundoff());
					}
					//std::cout << *this << std::endl;
					return true;
				}

				if (etimes == 3) {
					double A, B, C, D;
					A = eq.getTheta(3);
					B = eq.getTheta(2);
					C = eq.getTheta(1);
					D = eq.getTheta(0);
					double delta1 = B * C / (6 * A * A) - B * B * B / (27 * A * A * A) - D / (2 * A);
					double delta2 = C / (3 * A) - B * B / (9 * A * A);
					double delta = delta1 * delta1 + delta2 * delta2 * delta2;
					if (delta > 0) {
						std::cout << RED << "Delta shouldnot be more than 0.\n";
						return false;
					}
					double x1 = -B / (3 * A) + pow(delta1 + sqrt(delta), 1.0/3) + pow(delta1 - sqrt(delta), 1.0/3);
					//double x2, x3;
					// calculate x2 x3
					this->add((new Polynomial(x1, -1.0))->roundoff());
					return true;
				}
			} else {
				eq.factor();
			}

			return false;
		}
		
		static int solver(const Classifier* cl, Solution& sol) {
			if ((cl == NULL) || (cl->size == 0)) 
				return Polynomial::solver(NULL, sol);
			int select_equation = rand() % cl->size;
			return Polynomial::solver(&(cl->eqs[select_equation]), sol);
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
