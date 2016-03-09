#ifndef _STATES_H_
#define _STATES_H_
#include "config.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <string.h>


typedef double State[Nv];

class States{
	public:
		States() : max_size(Mitems) {
			values = new double[Mitems][Nv];
			t_index = new int[Mitems];
			t_index[0] = 0;
			p_index = 0;
			size = 0;
		}

		~States() {
			if (values != NULL) {
				delete[] values;
				values = NULL;
			}
			if (t_index != NULL) {
				delete[] t_index;
				t_index = NULL;
			}
		}

		/*bool setVariableName(char** strs) {
		  if (strs == NULL) return false;
		  for (int i = 0; i < VARS; i++){
		  if (strs[i] && strs[i][0] != '\0')
		  strncpy(name[i], strs[i], 8);
		  }
		  return true;
		  }*/

		bool initFromFile(int num, std::ifstream& fin) {
			int tmpint;
			char tmpchar;
			for (int i = 0; i < num; i++) {
				fin >> tmpint;
				for (int j = 0; j < Nv; j++) {
					fin >> tmpint >> tmpchar >> values[i][j];
					assert(tmpint == j);
					assert(tmpchar == ':');
				}
			}
			size += num;
			t_index[p_index + 1] = t_index[p_index] + num;
			p_index++;
			return true;
		}

		int addStates(State st[], int len) {
			if (size + len >= max_size) {
				//std::cerr << "exceed maximium program states." << std::endl;
				int previous_max_size = max_size;
				double(*previous_values)[Nv] = values;
				//while (t_index[p_index] + len >= max_size)
				max_size *= 2;

				if ((values = new double[max_size][Nv]) == NULL)
					return -1;
				memcpy(values, previous_values, previous_max_size * Nv * sizeof(double));


				int* previous_t_index = t_index;
				if ((t_index = new int[max_size]) == NULL)
					return -1;
				memcpy(t_index, previous_t_index, previous_max_size * sizeof(int));
			}

			int addLength = 0;
			for (int i = 0; i < len; i++) {
				// try to insert state st[i]
				bool skip = false;
				for (int j = 0; j < size; j++) {
					if (stateCmp(values[j], st[i]) == true) {
						skip = true;
						break;
					}
				}
				if (skip) continue;
				stateCpy(&values[size], &st[i]);
				addLength++;
				size++;
			}
			t_index[p_index + 1] = t_index[p_index] + addLength;
			p_index++;
			//std::cout << "+" << addLength << " ";
			return addLength;
		}

		inline int getTraceSize() {
			return p_index;
		}

		inline int traces_num() {
			return p_index;
		}

		int getSize() {
			return size;
		}

		void dumpTrace(int num) {
			if (num >= p_index) {
				std::cerr << "exceed state set boundary" << std::endl;
				return;
			}
			for (int i = t_index[num]; i < t_index[num + 1]; i++) {
				std::cout << "(" << values[i][0];
				for (int j = 1; j < Nv; j++)
					std::cout << "," << values[i][j];
				std::cout << ")->";
			}
			std::cout << "end.";
		}

		friend std::ostream& operator << (std::ostream& out, const States& ss) {
			//std::cout << "lable[" << ss.label << "]:" << std::endl;
			for (int i = 0; i < ss.p_index; i++) {
				std::cout << "\tTr." << i << ":";
				for (int j = ss.t_index[i]; j < ss.t_index[i + 1]; j++) {
					std::cout << "(" << ss.values[j][0];
					for (int k = 1; k < Nv; k++)
						std::cout << "," << ss.values[j][k];
					std::cout << ")->";
				}
				std::cout << "end." << std::endl;
			}
			return out;
		}

		double* getState (int i) {
			if (i >= size) return NULL;
			return values[i];
		}

		int getLabel(int index = 0) { 
			return label; 
		}

	public:
		//int num; /// = VARS
		/// is designed to store all the names of variables, used for output;
		///  currently not in use.
		/// char name[VARS][8];

		State (*values);
		int label;
		int size;

		// t_index is the array stored all the offset of traces in states.
		// e.g. t_index[0] = 0 means the 0-th trace is located at position 0 in values;
		// t_index[1] = 5 means the 1st trace is located at postion 5....
		//						and also the length of the 0-TH trace is 5!!!!!
		int* t_index;

		// p_index is the current position of the t_index.
		// Apperately, we have :
		//						t_index[p_index] == size
		// Thus, size is redundant in this context.
		int p_index;

	private:
		static bool stateCmp(const State& s1, const State& s2) {
			for (int i = 0; i < Nv; i++) {
				if (s1[i] != s2[i])
					return false;
			}
			return true;
		}
		static inline void stateCpy(State* dst, State* src, int length = 1) {
			memcpy(dst, src, sizeof(State) * length);
		}
		int max_size;
};

#endif
