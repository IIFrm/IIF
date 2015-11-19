#ifndef _STATES_H_
#define _STATES_H_
//#include "header.h"
#include "config.h"
#include "string.h"

#include <cstdlib>
#include <iostream>

class States{
	public:
		States() : max_items(100000) {
			values = new double [max_items][VARS];
			index = new int [max_items];
			index[0] = 0;
			p_index = 0;
		}

		//States(const States& ps) : max_items(10000) {
		//	std::cout << "clone constructor called." << __FILE__ << ":" << __LINE__ << std::endl;
		//}
		
		~States() {
			//std::cout << "--> destructor of label: " << label << std::endl;
			if (values != NULL) {
				delete[] values;
				values = NULL;
			}
			if (index != NULL) {
				delete[] index;
				index = NULL;
			}
			//std::cout << "<-- destructor of label: " << label << "[done]" << std::endl;
		}

		int add_states(double st[][VARS], int len){
			if (index[p_index] + len >= max_items) {
				std::cerr << "exceed maximium program states." << std::endl;
				exit(-1);
			}
			
			/*
			std::cout << "+++";
			for (int i = 0; i < len; i++)  {
				std::cout << "{";
				for (int j = 0 ; j < vars; j++)
					std::cout << st[i][j] << ",";
				std::cout << "}-";
			}
			std::cout << std::endl;
			*/

			memcpy(values[index[p_index]], st, sizeof(double) * VARS * len);
			//memmove(values[index[p_index]], st, sizeof(double) * vars * len);
			
			index[p_index+1] = index[p_index] + len;
			p_index++;
			return 0;
		}

		
		friend std::ostream& operator << (std::ostream& out, const States& ss){
			//std::cout << "lable[" << ss.label << "]:" << std::endl;
			for (int i = 0; i < ss.p_index; i++) {
				std::cout << "\tTr." << i << ":";
				for (int j = ss.index[i]; j < ss.index[i + 1]; j++) {
					std::cout << "(" << ss.values[j][0];
					for (int k = 1; k < VARS; k++)
						std::cout << "," << ss.values[j][k];
					std::cout << ")->";
				}
				std::cout << "end." << std::endl;
			}
			return out;
		}

		int traces_num() {
			return p_index;
		}

		int size() {
			return index[p_index];
		}

		void print_trace(int num) {
			if (num >= p_index) {
				std::cerr << "exceed state set boundary" << std::endl;
				return;
			}
			for (int i = index[num]; i < index[num + 1]; i++) {
				std::cout << "(" << values[i][0];
				for (int j = 1; j < VARS; j++)
					std::cout << "," << values[i][j];
				std::cout << ")->";
			}
			std::cout << "end.";
		}

		double (*values)[VARS];
		int* index;
		int p_index;
		int label;
	private:
		const int max_items;
};


#endif
