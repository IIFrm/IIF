/*************************************************************************
    > File Name: config.h
    > Author: Li Jiaying
    > Mail: lijiaying1989@gmail.com 
    > Created Time: 2015年11月05日 星期四 11时50分51秒
 ************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define __PRT
//#define VARS (1)

extern int (*target_program) (int*);
bool register_program(int (*func)(int*), char* func_name = 0);

#ifndef VARS 
#define VARS (2)
#endif

const int max_items = 100000;
const int q_items = 1000;

const int init_exes = 4 * VARS;
const int after_exes = 2 * VARS;
const int final_exes_times = 10; /* 10 times of current exes*/
const int random_exes = 2;
const int max_iter = 32;

#endif
