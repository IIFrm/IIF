#ifndef _IIF_LEARN_
#define _IIF_LEARN_

#include "config.h"
#include "states.h"
#include "equation.h"
#include "instrumentation.h"
#include "color.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>


class IIF_learn{
	public:
		IIF_learn(States* gsets, int (*func)(int*)): gsets(gsets), func(func) {
			init_gsets();
		}

		IIF_learn() {
			gsets = NULL;
			func = NULL;
		}


		void run_target(Solution& input) {
			assert(func != NULL);
			assert(&input != NULL);
			before_loop();

			int a[VARS];
			for (int i = 0; i < VARS; i++)
				a[i] = static_cast<int>(input.x[i]);
			//target_program
			func(a);

			after_loop(gsets);
			if (gsets[COUNT_EXAMPLE].traces_num() > 0) {
				set_console_color(std::cout, RED);
				std::cout << "}\nProgram BUG! Program have encountered a Counter-Example trace." << std::endl;
				std::cout << gsets[COUNT_EXAMPLE] << std::endl;
				unset_console_color(std::cout);
				exit(-1);
			}
		}

		virtual int learn() = 0;

	protected:
		void init_gsets() {
			gsets[NEGATIVE].label = -1;
			gsets[QUESTION].label = 0;
			gsets[POSITIVE].label = 1;
			gsets[COUNT_EXAMPLE].label = 2;
		}
		States* gsets;
		int (*func)(int*);
};

//void print_null(const char *s) {}


#endif
