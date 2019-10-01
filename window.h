
#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

/**
 *	@struct		window - a struct containing window information
 *
 *	@member		handle - a handle to the relevant window
 */
typedef struct window_type {
	
	HWND handle;
	int is_running;
} window;

/**
 *	@function	window_is_valid - returns nonzero if the window is valid
 *
 *	@param		t_window - the window the check for validity
 *	@returns	nonzero if the window is valid
 */
int window_is_valid(window* t_window);

/**
 *	@function	init_window - initializes the given window
 *
 *	@param		t_window - an invalid window to be initialized
 *	@param		t_proc - the windows procedure function for this window
 *	@returns	nonzero if initialized successfully
 */
int init_window(window* t_window, WNDPROC t_proc);

/**
 *	@function	final_window - finalizes the given window
 *
 *	@param		t_window - a valid window to be finalized
 */
void final_window(window* t_window);

#endif