#ifndef _COLOR_H_
#define _COLOR_H_
#include <iostream>

enum color {RED = 0, YELLOW, GREEN, BLUE, WHITE};
void set_console_color(std::ostream& out, int choice = YELLOW); 
void unset_console_color(std::ostream& out);

#endif
