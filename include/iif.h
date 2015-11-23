/** @file iif.h
 *  @brief Contains all the files that needed to be included by a new test.
 *  
 *  By include this file, it should resolve all the reference errors to the framework.
 *
 *  @author Li Jiaying
 *  @bug No found bugs
 */
#ifndef _IIF_H_
#define _IIF_H_

#include "config.h"
#include "instrumentation.h"
#include "ml_algo.h"
#include "svm.h"
#include "svm_i.h"
#include "color.h"
#include "equation.h"
#include "states.h"
#include "iif_learn.h"
#include "iif_svm_learn.h"
#include "iif_svm_i_learn.h"
#include "iif_assert.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <cstdlib>

//#define conj target_program
extern int minv, maxv;

/*#define create_state_sets(gsets) do{\
	States global_states_sets[4];\
	gsets = &global_states_sets[1];\
} while(0);

#define delete_state_sets(gsets) do {\
	States* p = &gsets[-1];\
	delete []p;\
} while(0);
*/

#endif
