
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

typedef struct shader_type {
	
	unsigned int id;
	
} shader;

/**
 *	@function	init_shader - initializes a shader
 *
 *	@param		t_shader - an invalid shader to be initialized
 *	@param		t_shader_type - the shader type
 *	@param		t_string_count - the number of strings in t_shader_stings
 *	@param		t_shader_strings - a t_string_count length array of char arrays representing the body of the shader
 *	@param		t_shader_lengths - a t_string_count length array of lengths of the char arrays within t_shader_strings
 *	@returns	nonzero if the shader is valid
 */
int init_shader(shader* t_shader, unsigned int t_shader_type, unsigned int t_string_count, const char** t_shader_strings, const int* t_shader_lengths);

/**
 *	@function	final_shader - finalizes a shader
 *
 *	@param		t_shader - a valid shader to be finalized
 */
void final_shader(shader* t_shader);

typedef struct program_type {
	
	unsigned int id;
	
	unsigned int shader_count;
	shader** shaders;
	
} program;

/**
 *	@function	final_program - initializes a program
 *
 *	@param		t_program - an invalid program to be initialized
 *	@param		t_shader_count - the number of shaders to link the program to
 *	@param		t_shaders - pointers to the valid shaders
 *	@returns	nonzero if the program is valid
 */
int init_program(program* t_program, unsigned int t_shader_count, shader** t_shaders);

/**
 *	@function	final_shader - finalizes a shader
 *
 *	@param		t_program - a valid shader to be finalized
 */
void final_program(program* t_program);

#endif