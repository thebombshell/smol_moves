
#include "window_renderer.h"
#include <assert.h>
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

int g_world_loc, g_view_loc, g_proj_loc;

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
	
	wglCreateContextAttribsARB = load_gl_function("wglCreateContextAttribsARB");
	
	context = wglCreateContextAttribsARB(g_device, 0, context_attribute_list);
	wglMakeCurrent(g_device, context);
	wglDeleteContext(g_context);
	g_context = context;
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

LRESULT CALLBACK window_proc(HWND t_handle, UINT t_message, WPARAM t_w_param, LPARAM t_l_param) {
	
	switch (t_message) {
		
		case WM_DESTROY:
		
			PostQuitMessage(0);
		return 0;
		
		case WM_KEYDOWN:
		
			g_is_key_down[t_w_param] = 1;
		return 0;
		
		case WM_KEYUP:
			
			g_is_key_down[t_w_param] = 0;
		return 0;
	}
	return DefWindowProc(t_handle, t_message, t_w_param, t_l_param);
}

void setup_window() {
	
	int major_version;
	int minor_version;
	int pixel_format;
	memset(g_is_key_down, 0, 256);
	WNDCLASS window_class = {};
	
    g_console = GetStdHandle(STD_OUTPUT_HANDLE);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = window_proc;
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

void alloc_string_from_file(const char* t_path, char** t_buffer, int* t_length) {
	
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

unsigned int create_shader(unsigned int t_shader_type, unsigned int t_string_count, const char** t_strings, const int* t_string_lengths) {
	
	assert(t_string_count > 0);
	
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
	
	assert(t_shader_count > 0);
	
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

unsigned int create_shader_from_file(const char* t_path, unsigned int t_shader_type, const char* t_header, int t_header_length) {
	
	static const char* null_string = "";
	
	char* shader_contents;
	int shader_contents_length;
	alloc_string_from_file(t_path, &shader_contents, &shader_contents_length);
	const char* strings[2] = { t_header ? t_header : null_string, shader_contents };
	const int lengths[2] = { t_header ? t_header_length : 0, shader_contents_length };
	
	unsigned int shader = create_shader(t_shader_type, 2, strings, lengths);
	free(shader_contents);
	
	return shader;
}

unsigned int create_program_from_files(int t_shader_count, const char** t_paths, const unsigned int* t_shader_types, const char* t_header, int t_header_length) {
	
	assert(t_shader_count > 0);
	
	unsigned int* shaders = malloc(sizeof(unsigned int) * t_shader_count);
	int i;
	for (i = 0; i < t_shader_count; ++i) {
		
		shaders[i] = create_shader_from_file(t_paths[i], t_shader_types[i], t_header, t_header_length);
	}
	int program = create_program(t_shader_count, shaders);
	for (i = 0; i < t_shader_count; ++i) {
		
		if (shaders[i]) {
			
			glDeleteShader(shaders[i]);
		}
	}
	free(shaders);
	return program;
}

const float* geometry_cube_get_vertices() {
	
	static const float vertices[24] =
		{ -0.5f, -0.5f, -0.5f
		, 0.5f, -0.5f, -0.5f
		, 0.5f, 0.5f, -0.5f
		, -0.5f, 0.5f, -0.5f
		, -0.5f, -0.5f, 0.5f
		, 0.5f, -0.5f, 0.5f
		, 0.5f, 0.5f, 0.5f
		, -0.5f, 0.5f, 0.5f };
	
	return vertices;
}

const unsigned int* geometry_cube_get_indices() {
	
	static const unsigned int indices[36] = 
		{ 0, 3, 2, 0, 2, 1 
		, 4, 5, 6, 4, 6, 7
		, 0, 1, 5, 0, 5, 4
		, 1, 2, 6, 1, 6, 5
		, 2, 3, 7, 2, 7, 6
		, 3, 0, 4, 3, 4, 7 };
		
	return indices;
}

unsigned int geometry_cube_get_vertex_count() {
	
	return 8;
}

unsigned int geometry_cube_get_index_count() {
	
	return 36;
}

void check_gl_error() {
	
	unsigned int error = glGetError();
	
	if (error != GL_NO_ERROR) {

		printf_yellow("[gl error]: ");
	}
	switch (error) {
		case GL_INVALID_ENUM:
		
			printf_red("Invalid Enum!\n");
		break;
		case GL_INVALID_VALUE:
		
			printf_red("Invalid Value!\n");
		break;
		case GL_INVALID_OPERATION:
		
			printf_red("Invalid Enum!\n");
		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
		
			printf_red("Invalid Enum!\n");
		break;
		case GL_OUT_OF_MEMORY:
		
			printf_red("Invalid Enum!\n");
		break;
		case GL_STACK_UNDERFLOW:
		
			printf_red("Invalid Enum!\n");
		break;
		case GL_STACK_OVERFLOW:
		
			printf_red("Invalid Enum!\n");
		break;
	}
}
