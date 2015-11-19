#include "color.h"

#ifdef WIN32
#include <windows.h>
void set_console_color(std::ostream& out, int choice) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (choice) {
		case RED: // red
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case YELLOW: // yellow
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case GREEN: // green
			SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case BLUE: // blue
			SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case WHITE: // white
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
	}
}
#endif


#ifdef linux
void set_console_color(std::ostream& out, int choice) {
	switch (choice) {
		case RED: // red
			out << "\033[33;\x1b[31m";
			break;
		case YELLOW: // yellow
			out << "\033[33;\x1b[33m";
			break;
		case GREEN: // green
			out << "\033[33;\x1b[32m";
			break;
		case BLUE: // blue
			out << "\033[33;\x1b[34m";
			break;
		case WHITE: // white
			out << "\033[0m";
			break;
	}	
}
#endif


#ifdef __MACH__
void set_console_color(std::ostream& out, int choice) {
	switch (choice) {
		case RED: // red
			out << "\033[33;\x1b[31m";
			break;
		case YELLOW: // yellow
			out << "\033[33;\x1b[33m";
			break;
		case GREEN: // green
			out << "\033[33;\x1b[32m";
			break;
		case BLUE: // blue
			out << "\033[33;\x1b[34m";
			break;
		case WHITE: // white
			out << "\033[0m";
			break;
	}	
}
#endif


void unset_console_color(std::ostream& out) {
	set_console_color(out, WHITE);
}

