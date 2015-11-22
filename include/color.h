#ifndef _COLOR_H_
#define _COLOR_H_
#include <iostream>

/**
 * \enum color
 * This is color predifined in project.
 * Here we only introduce RED, YELLOW, GREEN, BLUE, WHITE 
 * which is enough for our output.
 */
enum color {RED = 0, YELLOW, GREEN, BLUE, WHITE};

/**
 * \set_console_color
 * set the given stream to a given color, YELLOW is default.
 * @param1 : ostream, defines which stream you want to set
 * @param2 : The Color to use. You can import more color if you want.
 */
void set_console_color(std::ostream& out, int choice = YELLOW); 

/**
 * \unset_console_color
 * set the console color back to origin
 * By origin, we mean black background, white foreground, not strong comparasion
 */
void unset_console_color(std::ostream& out);

#endif
