
#ifndef WINDOW_RENDERER_H
#define WINDOW_RENDERER_H

#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#ifndef RENDERER_BUFFER_COUNT
#define RENDERER_BUFFER_COUNT 256
#endif

#ifndef RENDERER_VERTEX_ARRAY_COUNT
#define RENDERER_VERTEX_ARRAY_COUNT 64
#endif

#ifndef RENDERER_MAX_DRAW_COUNT
#define RENDERER_MAX_DRAW_COUNT 256
#endif

HWND g_window;
HDC g_device;
HANDLE g_console;
HGLRC g_context;

signed char g_is_key_down[256];

#define printf_red(...) SetConsoleTextAttribute(g_console, FOREGROUND_RED); printf(__VA_ARGS__);
#define printf_green(...) SetConsoleTextAttribute(g_console, FOREGROUND_GREEN); printf(__VA_ARGS__);
#define printf_blue(...) SetConsoleTextAttribute(g_console, FOREGROUND_BLUE); printf(__VA_ARGS__);
#define printf_yellow(...) SetConsoleTextAttribute(g_console, FOREGROUND_RED | FOREGROUND_GREEN); printf(__VA_ARGS__);
#define printf_cyan(...) SetConsoleTextAttribute(g_console, FOREGROUND_GREEN | FOREGROUND_BLUE); printf(__VA_ARGS__);
#define printf_magenta(...) SetConsoleTextAttribute(g_console, FOREGROUND_RED | FOREGROUND_BLUE); printf(__VA_ARGS__);
#define printf_white(...) SetConsoleTextAttribute(g_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); printf(__VA_ARGS__);

PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENBUFFERSPROC glGenBuffers;

PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;

PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLSHADERSOURCEPROC glShaderSource;

PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

void check_gl_error();

unsigned long long get_time_ms();

void setup_window();
void shutdown_window();

void alloc_string_from_file(const char* t_path, char** t_buffer, int* t_length);
unsigned int create_shader(unsigned int t_shader_type, unsigned int t_string_count, const char** t_strings, const int* t_string_lengths);
unsigned int create_program(unsigned int t_shader_count, unsigned int* t_shaders);
unsigned int create_shader_from_file(const char* t_path, unsigned int t_shader_type, const char* t_header, int t_header_length);
unsigned int create_program_from_files(int t_shader_count, const char** t_paths, const unsigned int* t_shader_types, const char* t_header, int t_header_length);

const float* geometry_cube_get_vertices();
const unsigned int* geometry_cube_get_indices();
unsigned int geometry_cube_get_vertex_count();
unsigned int geometry_cube_get_index_count();


#endif