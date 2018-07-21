
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "window.h"
#include "glad.h"

/**
 *	@struct		graphics - a struct containign graphical context information
 *
 *	@member		surface - the window used to create the graphical context
 *	@member		device - the device the graphical context is bound to
 *	@member		context - the graphical context
 */
typedef struct graphics_type {
	
	window* surface;
	HDC device;
	HGLRC context;
} graphics;

/**
 *	@function	graphics_is_valid - returns nonzero if the given graphics is valid
 *
 *	@param		t_graphics - the graphics to check for validity
 *	@returns	nonzero if the graphics is valid
 */
int graphics_is_valid(graphics* t_graphics);

/**
 *	@function	init_graphics - initialize the given graphics
 *
 *	@param		t_graphics - an invalid graphics to be initialized
 *	@param		t_window - a valid window to get the graphical device from
 *	@returns	nonzero if initialized successfully
 */
int init_graphics(graphics* t_graphics, window* t_window);

/**
 *	@function	final_graphics - finalize the given graphics
 *
 *	@param		t_graphics - a valid graphics to be finalized
 */
void final_graphics(graphics* t_graphics);

/**
 *	@function	graphics_swap_buffers - swaps the backbuffer and forebuffer
 *
 *	@param		t_graphics - a valid graphics to swap the buffers of
 */
void graphics_swap_buffers(graphics* t_graphics);

#endif