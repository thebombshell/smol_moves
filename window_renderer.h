
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
float g_camera_eye[3];
float g_camera_look[3];
float g_camera_up[3];
unsigned int g_buffers[RENDERER_BUFFER_COUNT];
unsigned int g_vertex_arrays[RENDERER_BUFFER_COUNT];
unsigned int g_draw_vertex_arrays[RENDERER_MAX_DRAW_COUNT];
unsigned int g_draw_modes[RENDERER_MAX_DRAW_COUNT];
unsigned int g_draw_types[RENDERER_MAX_DRAW_COUNT];
unsigned int g_draw_counts[RENDERER_MAX_DRAW_COUNT];
unsigned int g_draw_count;

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

PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

unsigned long long get_time_ms();

void setup_window(WNDPROC);
void shutdown_window();

void file_as_string_alloc(const char* t_path, char** t_buffer, int* t_length);
unsigned int create_shader(unsigned int t_shader_type, unsigned int t_string_count, const char** t_strings, int* t_string_lengths);
unsigned int create_program(unsigned int t_shader_count, unsigned int* t_shaders);

void setup_renderer();
void shutdown_renderer();

void queue_draw(unsigned int t_vertex_array, unsigned int t_mode, unsigned int t_type, unsigned int t_count);
void render();

#endif