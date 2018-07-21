
#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef NDEBUG
#define PROLOGUE() ((void) 0)
#else
#define PROLOGUE() printf("[%s:%i]", __FILE__, __LINE__)
#endif

#if defined _WIN32

#include <windows.h>

#define SET_BLACK() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0)
#define SET_RED() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED)
#define SET_GREEN() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN)
#define SET_BLUE() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE)
#define SET_YELLOW() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN)
#define SET_MAGENTA() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE)
#define SET_CYAN() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE)
#define SET_WHITE() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#define SYS(...) SET_GREEN(); PROLOGUE(); printf( __VA_ARGS__ ); SET_WHITE(); printf("\n")
#define LOG(...) SET_WHITE(); PROLOGUE(); printf( __VA_ARGS__ ); SET_WHITE(); printf("\n")
#define WRN(...) SET_YELLOW(); PROLOGUE(); printf( __VA_ARGS__ ); SET_WHITE(); printf("\n")
#define ERR(...) SET_RED(); PROLOGUE(); printf( __VA_ARGS__ ); SET_WHITE(); printf("\n")

#elif defined __EMSCRIPTEN__

#define LOG(...) PROLOGUE(); printf( __VA_ARGS__ ); printf("\n")
#define WRN(...) PROLOGUE(); printf( __VA_ARGS__ ); printf("\n")
#define ERR(...) PROLOGUE(); printf( __VA_ARGS__ ); printf("\n")

#endif


#endif