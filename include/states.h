#ifndef _STATES_H_
#define _STATES_H_
#include "config.h"
#include <iostream>


class States{
	public:
		States();
		
		~States();

		int add_states(double st[][VARS], int len);

		int traces_num();

		int size();

		void print_trace(int num);

		friend std::ostream& operator << (std::ostream& out, const States& ss);

	public:
		double (*values)[VARS];
		//vector<vector<double>> values; // (max_size, vector<double>(VARS));
		int* index;
		//vector<int> index; // (max_size);	
		int p_index;
		int label;

	private:
		int max_size;
};

#endif
