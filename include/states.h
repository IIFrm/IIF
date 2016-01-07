#ifndef _STATES_H_
#define _STATES_H_
#include "config.h"
#include <iostream>
#include <string.h>


typedef double State[VARS];

class States{
	public:
		States() : max_size(max_items) {
			values = new double[max_items][VARS];
			t_index = new int[max_items];
			//values = vector< vector<double> > (max_size, vector<double>(VARS));
			//index= vector<int>(max_size);
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

		int addStates(State st[], int len) {
			if (size + len >= max_size) {
				//std::cerr << "exceed maximium program states." << std::endl;
				int previous_max_size = max_size;
				double(*previous_values)[VARS] = values;
				//while (t_index[p_index] + len >= max_size)
					max_size *= 2;

				if ((values = new double[max_size][VARS]) == NULL)
					return -1;
				memcpy(values, previous_values, previous_max_size * VARS * sizeof(double));


				int* previous_t_index = t_index;
				if ((t_index = new int[max_size]) == NULL)
					return -1;
				memcpy(t_index, previous_t_index, previous_max_size * sizeof(int));
			}

			memcpy(values[size], st, sizeof(double) * VARS * len);
			//memmove(values[index[p_index]], st, sizeof(double) * vars * len);
			t_index[p_index + 1] = t_index[p_index] + len;
			p_index++;
			size = size + len;
			return len;
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
				for (int j = 1; j < VARS; j++)
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
					for (int k = 1; k < VARS; k++)
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
		int max_size;
};

#endif
