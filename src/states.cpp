#include "config.h"
#include "string.h"
#include "states.h"
#include <cstdlib>
#include <vector>
#include <iostream>
//using namespace std;

States::States() : max_size(10000) {
	values = new double[max_items][VARS];
	index = new int[max_items];
	//values = vector< vector<double> > (max_size, vector<double>(VARS));
	//index= vector<int>(max_size);
	index[0] = 0;
	p_index = 0;
}


States::~States() {
	//std::cout << "--> destructor of label: " << label << std::endl;
	//values.clear();
	//index.clear();

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


int States::add_states(double st[][VARS], int len) {
	if (index[p_index] + len >= max_size) {
		//std::cerr << "exceed maximium program states." << std::endl;
		int previous_max_size = max_size;
		while (index[p_index] + len >= max_size)
			max_size *= 2;
		double(*previous_values)[VARS] = values;
		if ((values = new double[max_size][VARS]) == NULL)
			return -1;
		memcpy(values, previous_values, previous_max_size * VARS * sizeof(double));


		int* previous_index = index;
		if ((index = new int[max_size]) == NULL)
			return -1;
		memcpy(index, previous_index, previous_max_size * sizeof(int));
		//values.resize(max_size);
		//index.resize(max_size);
		//exit(-1);
	}

	/*
	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < VARS; j++)
			values[index[p_index] + i][j] = st[i][j];
	}
	*/
	memcpy(values[index[p_index]], st, sizeof(double) * VARS * len);
	//memmove(values[index[p_index]], st, sizeof(double) * vars * len);
	index[p_index + 1] = index[p_index] + len;
	p_index++;
	return 0;
}

int States::traces_num() { return p_index; }

int States::size() { return index[p_index]; }

void States::print_trace(int num) {
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

std::ostream& operator << (std::ostream& out, const States& ss) {
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
