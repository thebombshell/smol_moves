
#include "window_renderer.h"
#include <sys/timeb.h>

const int context_attribute_list[] = 
	{ WGL_CONTEXT_MAJOR_VERSION_ARB, 3
	, WGL_CONTEXT_MINOR_VERSION_ARB, 3
	, WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB 
	, 0 };

const int pixel_format_attribute_list[] =
	{ WGL_DRAW_TO_WINDOW_ARB, GL_TRUE
	, WGL_SUPPORT_OPENGL_ARB, GL_TRUE
	, WGL_DOUBLE_BUFFER_ARB, GL_TRUE
	, WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB
	, WGL_COLOR_BITS_ARB, 32
	, WGL_DEPTH_BITS_ARB, 24
	, WGL_STENCIL_BITS_ARB, 8
	, 0 };
	
PIXELFORMATDESCRIPTOR pixel_format_description = 
	{ sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA
	, 32, 0, 0, 0, 0, 0, 0, 0, 0
	, 0, 0, 0, 0, 0
	, 24, 8, 0,
	PFD_MAIN_PLANE, 0, 0, 0, 0 };
	
unsigned long long get_time_ms() {
	
	struct timeb time;
	ftime(&time);
	return 1000 * (unsigned long long)(time.time) + time.millitm;
}

void* load_gl_function(const char* t_name) {
	
	void* function_pointer = (void*)wglGetProcAddress(t_name);
	if ((long long)function_pointer < 3 && (long long)function_pointer > -1) {
		
		printf("Failed to load function %s", t_name);
	}
	return function_pointer;
}

void setup_gl() {
	
	HGLRC context;
	
	glBindBuffer = load_gl_function("glBindBuffer");
	glBufferData = load_gl_function("glBufferData");
	glDeleteBuffers = load_gl_function("glDeleteBuffers");
	glGenBuffers = load_gl_function("glGenBuffers");
	
	glBindVertexArray = load_gl_function("glBindVertexArray");
	glDeleteVertexArrays = load_gl_function("glDeleteVertexArrays");
	glGenVertexArrays = load_gl_function("glGenVertexArrays");
	
	glAttachShader = load_gl_function("glAttachShader");
	glCompileShader = load_gl_function("glCompileShader");
	glCreateShader = load_gl_function("glCreateShader");
	glDeleteShader = load_gl_function("glDeleteShader");
	glGetShaderInfoLog = load_gl_function("glGetShaderInfoLog");
	glGetShaderiv = load_gl_function("glGetShaderiv");
	glShaderSource = load_gl_function("glShaderSource");
	
	glCreateProgram = load_gl_function("glCreateProgram");
	glDeleteProgram = load_gl_function("glDeleteProgram");
	glGetProgramInfoLog = load_gl_function("glGetProgramInfoLog");
	glGetProgramiv = load_gl_function("glGetProgramiv");
	glLinkProgram = load_gl_function("glLinkProgram");
	glUseProgram = load_gl_function("glUseProgram");
	
	glGetUniformLocation = load_gl_function("glGetUniformLocation");
	glUniformMatrix2fv = load_gl_function("glUniformMatrix2fv");
	glUniformMatrix3fv = load_gl_function("glUniformMatrix3fv");
	glUniformMatrix4fv = load_gl_function("glUniformMatrix4fv");
	
	
	glDisableVertexAttribArray = load_gl_function("glDisableVertexAttribArray");
	glEnableVertexAttribArray = load_gl_function("glEnableVertexAttribArray");
	glVertexAttribPointer = load_gl_function("glVertexAttribPointer");
	
	glDrawArraysIndirect = load_gl_function("glDrawArraysIndirect");
	glDrawElementsIndirect = load_gl_function("glDrawElementsIndirect");
	glMultiDrawArraysIndirect = load_gl_function("glMultiDrawArraysIndirect");
	glMultiDrawElementsIndirect = load_gl_function("glMultiDrawElementsIndirect");
	
	wglCreateContextAttribsARB = load_gl_function("wglCreateContextAttribsARB");
	
	context = wglCreateContextAttribsARB(g_device, 0, context_attribute_list);
	wglMakeCurrent(g_device, context);
	wglDeleteContext(g_context);
	g_context = context;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void setup_window(WNDPROC t_window_proc) {
	
	int major_version;
	int minor_version;
	int pixel_format;
	WNDCLASS window_class = {};
	
    g_console = GetStdHandle(STD_OUTPUT_HANDLE);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = t_window_proc;
	window_class.hInstance = GetModuleHandle(0);
	window_class.hIcon = LoadIcon(window_class.hInstance, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(0, IDC_ARROW);
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = "win32_classname";
	RegisterClass(&window_class);
	g_window = CreateWindowEx( 0, "win32_classname", "shader_viewer", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU
		, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, window_class.hInstance, 0);
	if (!g_window) {
		
		printf_red("Failed to create window!\n");
	}
	
    g_device = GetDC(g_window);
	pixel_format = ChoosePixelFormat(g_device, &pixel_format_description);
	if (!pixel_format) {
		
		printf_red("Failed to choose pixel format!\n");
	}
	if (!SetPixelFormat(g_device, pixel_format, &pixel_format_description)) {
		
		printf_red("Failed to set pixel format!\n");
	}
    DescribePixelFormat(g_device, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_description);
	
    g_context = wglCreateContext(g_device);
    wglMakeCurrent(g_device, g_context);
	setup_gl();
	
	glGetIntegerv(GL_MAJOR_VERSION, &major_version);
	glGetIntegerv(GL_MAJOR_VERSION, &minor_version);
	printf_green("OpenGl Version %u.%u\n", major_version, minor_version);
	
	ShowWindow(g_window, SW_SHOW);
}

void shutdown_window() {
	
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(g_window, g_device);
    wglDeleteContext(g_context);
	DestroyWindow(g_window);
}

void file_as_string_alloc(const char* t_path, char** t_buffer, int* t_length) {
	
	*t_buffer = 0;
	*t_length = 0;
	
	FILE* file = fopen(t_path, "rb");
	if (file == NULL) {
		
		printf_yellow("Failed to load file to string \"%s\"\n", t_path);
		return;
	}
	
	fseek(file, 0, SEEK_END);
	*t_length = ftell(file);
	fseek(file, 0, SEEK_SET);
	*t_buffer = malloc(*t_length);
	fread(*t_buffer, 1, *t_length, file);
	fclose(file);
}

unsigned int create_shader(unsigned int t_shader_type, unsigned int t_string_count, const char** t_strings, int* t_string_lengths) {
	
	int is_successful;
	unsigned int shader = glCreateShader(t_shader_type);
	glShaderSource(shader, t_string_count, t_strings, t_string_lengths);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_successful);
	if (!is_successful) {
		
		int log_length;
		char* info_log;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		info_log = malloc(log_length);
		glGetShaderInfoLog(shader, log_length, &log_length, info_log);
		printf_yellow("Failed to compile shader: ");
		printf_red("%s\n", info_log);
		free(info_log);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

unsigned int create_program(unsigned int t_shader_count, unsigned int* t_shaders) {
	
	unsigned int program = glCreateProgram();
	unsigned int i = 0;
	int is_successful;
	for (i = 0; i < t_shader_count; ++i) {
		
		glAttachShader(program, t_shaders[i]);
	}
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &is_successful);
	if (!is_successful) {
		
		int log_length;
		char* info_log;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
		info_log = malloc(log_length);
		glGetProgramInfoLog(program, log_length, &log_length, info_log);
		printf_yellow("Failed to link program: ");
		printf_red("%s\n", info_log);
		free(info_log);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

void setup_renderer() {
	
	g_camera_eye[0] = 0.0f;
	g_camera_eye[1] = 0.0f;
	g_camera_eye[2] = 0.0f;
	
	g_camera_look[0] = 0.0f;
	g_camera_look[1] = 1.0f;
	g_camera_look[2] = 0.0f;
	
	g_camera_up[0] = 0.0f;
	g_camera_up[1] = 0.0f;
	g_camera_up[2] = 1.0f;
	
	glGenBuffers(RENDERER_BUFFER_COUNT, g_buffers);
	glGenVertexArrays(RENDERER_VERTEX_ARRAY_COUNT, g_vertex_arrays);
	
	g_draw_count = 0;
	
	float vertices[24] = 
		{ -0.5f, -0.5f, 0.0f
		, 0.5f, -0.5f, 0.0f
		, 0.5f, 0.5f, 0.0f
		, -0.5f, 0.5f, 0.0f
		
		, -0.5f, -0.5f, 1.0f
		, 0.5f, -0.5f, 1.0f
		, 0.5f, 0.5f, 1.0f
		, -0.5f, 0.5f, 1.0f };
	unsigned int indices[36] = 
		{ 0, 3, 2, 0, 2, 1 
		, 4, 5, 6, 4, 6, 7
		, 0, 1, 5, 0, 5, 4
		, 1, 2, 6, 1, 6, 5
		, 2, 3, 7, 2, 7, 6
		, 3, 0, 4, 3, 4, 7};
	unsigned int indirects[5] =	{ 36, 1, 0, 0, 0 };
	glBindVertexArray(g_vertex_arrays[0]);
	glBindBuffer(GL_ARRAY_BUFFER, g_buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffers[1]);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, indices, GL_STATIC_DRAW);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(unsigned int) * 5, indirects, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	glBindVertexArray(0);
	queue_draw(g_vertex_arrays[0], GL_TRIANGLES, GL_UNSIGNED_INT, 1);
	
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void shutdown_renderer() {
	
	glDeleteBuffers(RENDERER_BUFFER_COUNT, g_buffers);
}

void queue_draw( unsigned int t_vertex_array, unsigned int t_mode, unsigned int t_type, unsigned int t_count) {
	
	g_draw_vertex_arrays[g_draw_count] = t_vertex_array;
	g_draw_modes[g_draw_count] = t_mode;
	g_draw_types[g_draw_count] = t_type;
	g_draw_counts[g_draw_count] = t_count;
	++g_draw_count;
}

void render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	int i;
	for (i = 0; i < g_draw_count; ++i) {
		
		glBindVertexArray(g_vertex_arrays[i]);
		glMultiDrawElementsIndirect(g_draw_modes[i], g_draw_types[i], 0, g_draw_counts[i], 0);
	}
	
	SwapBuffers(g_device);
}