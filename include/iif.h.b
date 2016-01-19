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
#include "solution.h"
#include "equation.h"
#include "states.h"
#include "base_learner.h"
#include "linear_learner.h"
#include "conjunctive_learner.h"
#include "disjunctive_learner.h"
#include "learner_pipeline.h"
#include "iif_assert.h"

#include <iostream>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <cstdlib>
/*
#include <signal.h>
#ifdef linux
#include <sys/time.h>
#include <unistd.h>
#endif
*/
//#define conj target_program
extern int minv, maxv;


/*States* initSystem(int (*func)(int*), const char* func_name = "UserFunction", int timeout = 60);

int destroySystem(States* gsets);
*/
#endif