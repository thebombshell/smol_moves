
#include "graphics.h"

#include "log.h"

#include <assert.h>
#include <GL/wglext.h>

const PIXELFORMATDESCRIPTOR g_pixel_format_descriptor = {
	sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA, 32, 8, 0, 8, 0, 8, 0, 8, 0, 0, 0, 0, 0, 0, 24, 8, 0, 0, 0, 0, 0, 0
};

const int g_graphics_attributes[7] =
	{ WGL_CONTEXT_MAJOR_VERSION_ARB, 4
	, WGL_CONTEXT_MINOR_VERSION_ARB, 6
    , WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
	, 0
	};

int graphics_is_valid(graphics* t_graphics) {
	
	return t_graphics && window_is_valid(t_graphics->surface);
}

int init_graphics(graphics* t_graphics, window* t_window) {
	
	assert(t_graphics);
	
	HGLRC temp;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	int pixel_format;
	
	t_graphics->surface = t_window;
	t_graphics->device = GetDC(t_window->handle);
	
	pixel_format = ChoosePixelFormat(t_graphics->device, &g_pixel_format_descriptor);
	if (!SetPixelFormat(t_graphics->device, pixel_format, &g_pixel_format_descriptor)) {
		
		ERR("failed to set pixel format");
		return 0;
	}
	
	temp = wglCreateContext(t_graphics->device);
	wglMakeCurrent(t_graphics->device, temp);
	
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB) {
		
		ERR("failed to load wglCreateContextAttribsARB: %ld", GetLastError());
		return 0;
	}
	
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB) {
		
		ERR("failed to load wglChoosePixelFormatARB: %ld", GetLastError());
		return 0;
	}
	
	if (!gladLoadGL()) {
		
		ERR("failed to load gl via glad");
		return 0;
	}
	wglMakeCurrent(0, 0);
	wglDeleteContext(temp);
	
	t_graphics->context = wglCreateContextAttribsARB(t_graphics->device, 0, &g_graphics_attributes[0]);
	if (!t_graphics->context) {
		
		ERR("failed to create graphics context");
		return 0;
	}
	
	wglMakeCurrent(t_graphics->device, t_graphics->context);
	LOG( "OpenGL %s - glsl %s - %s - %s"
		, glGetString(GL_VERSION)
		, glGetString(GL_SHADING_LANGUAGE_VERSION)
		, glGetString(GL_VENDOR)
		, glGetString(GL_RENDERER));
	return 1;
}

void final_graphics(graphics* t_graphics) {
	
	assert(graphics_is_valid(t_graphics));
	
	wglMakeCurrent(0, 0);
	wglDeleteContext(t_graphics->context);
}

void graphics_swap_buffers(graphics* t_graphics) {
	
	SwapBuffers(t_graphics->device);
}

int init_shader
	( shader* t_shader
	, unsigned int t_shader_type
	, unsigned int t_string_count
	, const char** t_shader_strings
	, const int* t_shader_lengths) {
	
	assert(t_shader);
	
	int is_compiled;
	int err_length;
	
	t_shader->id = glCreateShader(t_shader_type);
	
	glShaderSource(t_shader->id, t_string_count, t_shader_strings, t_shader_lengths);
	glCompileShader(t_shader->id);
	
	glGetShaderiv(t_shader->id, GL_INFO_LOG_LENGTH, &is_compiled);
	if (!is_compiled) {
		
		char err_log[1024];
		
		glGetShaderInfoLog(t_shader->id, 1024, &err_length, err_log);
		
		ERR("failed to compile shader: %s", err_log);
		
		glDeleteShader(t_shader->id);
		
		return 0;
	}
	return 1;
}

void final_shader(shader* t_shader) {
	
	assert(t_shader);
	
	glDeleteShader(t_shader->id);
}

int init_program(program* t_program, unsigned int t_shader_count, shader** t_shaders) {
	
	assert(t_program);
	
	int is_linked;
	int err_length;
	unsigned int i;
	
	t_program->shader_count = t_shader_count;
	t_program->shaders = t_shaders;
	
	t_program->id = glCreateProgram();
	
	for (i = 0; i < t_shader_count; ++i) {
		
		glAttachShader(t_program->id, t_shaders[i]->id);
	}
	
	glLinkProgram(t_program->id);
	
	glGetProgramiv(t_program->id, GL_LINK_STATUS, &is_linked);
	if (!is_linked) {
		
		char err_log[1024];
		
		glGetProgramInfoLog(t_program->id, 1024, &err_length, err_log);
		
		ERR("failed to link program: %s", err_log);
		
		glDeleteProgram(t_program->id);
		
		return 0;
	}
	return 1;
}

void final_program(program* t_program) {
	
	assert(t_program);
	
	glDeleteProgram(t_program->id);
}
