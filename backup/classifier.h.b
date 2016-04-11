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
		Polynomial* polys;
		Connector* cts;

	public:
		int size;
		int max_size;
		Classifier(int maxsize = 32) {
			max_size = maxsize;
			polys = new Polynomial[max_size];
			cts = new Connector[max_size];
			size = 0;
		}

		~Classifier() { 
			if (polys) delete []polys;
			if (cts) delete []cts;
		} 

		int clear() {
			size = 0;
			return 0;
		}

		Polynomial* operator[] (int index) {
			if ((index >=0) && (index < size))
				return &polys[index];
			else
				return NULL;
		}

		Polynomial* operator[] (int index) const {
			if ((index >=0) && (index < size))
				return &polys[index];
			else
				return NULL;
		}

		Classifier& operator--(int n) {
			if (size >= 1)
				size -= 1;
			else
				size = 0;
			return *this;
		}

		Classifier& operator+= (Polynomial& poly) {
			//factor(poly);
			add(poly, CONJUNCT);
			return *this;
		}

		Classifier& operator= (Polynomial& poly) {
			clear();
			add(poly, CONJUNCT);
			return *this;
		}

		Classifier& operator= (Classifier& rhs) {
			size = rhs.size;
			for (int i = 0; i < rhs.size; i++) {
				polys[i] = *rhs[i];
				cts[i] = rhs.cts[i];
			}
			return *this;
		}

		int add(Polynomial& poly, Connector& ct) {
			if (size >= max_size)
				return -1;
			polys[size] = poly;
			cts[size] = ct;
			return ++size;
		}

		int add(Polynomial& poly, int type = CONJUNCT) {
			if (size >= max_size)
				return -1;
			polys[size] = poly;
			cts[size].setType(type);
			return ++size;
		}

		bool factor(Polynomial& poly) {
			//std::cout << "\tFactorization Process >>>>\n";
			//std::cout << GREEN << poly << WHITE <<std::endl;
			int etimes = poly.getEtimes();
			if (etimes == 1) {
				//std::cout << "univariant linear function: " << poly << "\n";
				this->add(poly);
				return true;
			}

			if (Nv == 1) {
				if (etimes == 2) {
					// univariant quadratic function
					//std::cout << "univariant quadratic function: " << poly << "\n";
					double A, B, C;
					A = poly.getTheta(2);
					B = poly.getTheta(1);
					C = poly.getTheta(0);
					if (A == 0) {
						poly.setEtimes(1);
						this->add(poly);
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
						this->add(*(new Polynomial(x1, -1.0)));
						this->add(*(new Polynomial(-1 * x2, 1.0)), DISJUNCT);
					} else {
						this->add(*(new Polynomial(-1 * x1, 1.0)));
						this->add(*(new Polynomial(x2, -1.0)));
					}
					//std::cout << *this << std::endl;
					return true;
				}
			} else {
				this->add(poly);
				//poly.factor();
				return true;
			}

			return false;
		}

		static int solver(const Classifier* cl, Solution& sol) {
			if ((cl == NULL) || (cl->size == 0)) 
				return Polynomial::solver(NULL, sol);
			return Polynomial::solver((*cl)[rand() % cl->size], sol);
		}

		bool roundoff() {
			for (int i = 0; i < size; i++) 
				polys[i].roundoff();
			return true;
		}

		// checkint whether the last polynomial can infer the others
		bool resolveUniImplication() {
			if (size <= 1) return true;
#ifdef __PRT_SVM_I
			std::cout << "Resolving implication in Classifier, size = " << size;
#endif
			int last = size - 1;
			for (int j = 0; j < size - 1; j++) {
				if (polys[last].uniImply(polys[j]) == true) {
					for (int k = j; k < size; k++)
						polys[k] = polys[k+1];
					size--;
					j--;
				}
			}
#ifdef __PRT_SVM_I
			std::cout << "->" << size << std::endl;
#endif
			return true;
		}


		std::string toString() const {
			std::ostringstream stm;
			if (size <= 0) {
				stm << "";
				return stm.str();
			}
			stm << "(" << polys[0] << ") ";
			for (int i = 1; i < size; i++) 
				stm << cts[i] << " (" << polys[i] << ") ";
			return stm.str();
		}

		friend std::ostream& operator << (std::ostream& out, const Classifier& cs) {
			if (cs.size <= 0) {
				out << "true";
				return out;
			}
			if (cs.size == 1) {
				out << *cs[0];
				return out;
			}
			out << " (" << *cs[0] << ") ";
			for (int i = 1; i < cs.size; i++) 
				out << cs.cts[i] << " (" << *cs[i] << ") ";
			return out;
		}
};


#endif
