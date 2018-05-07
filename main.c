
#include "window_renderer.h"
#include "vector_math.h"
#include <stdio.h>
#include <string.h>

unsigned long g_fps;

LRESULT CALLBACK window_proc(HWND t_handle, UINT t_message, WPARAM t_w_param, LPARAM t_l_param) {
	
	switch (t_message) {
		
		case WM_DESTROY:
		
			PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(t_handle, t_message, t_w_param, t_l_param);
}

int main(void) {
	
	unsigned int basic_program;
	unsigned int basic_shaders[2];
	unsigned long long begin;
	unsigned long counter;
	unsigned long long diff;
	char* defines = "#version 330\n#define MUSE_WORLD_MATRIX\n#define USE_VIEW_MATRIX\n#define USE_PROJ_MATRIX\n";
	int defines_length = strlen(defines);
	char* frag_basic;
	int frag_basic_length;
	unsigned long long last;
	MSG message;
	unsigned long long start;
	char* vert_basic;
	int vert_basic_length;
	
	setup_window(window_proc);
	setup_renderer();
	
	file_as_string_alloc("shaders/frag_basic.glsl", &frag_basic, &frag_basic_length);
	file_as_string_alloc("shaders/vert_basic.glsl", &vert_basic, &vert_basic_length);
	char* frag_basic_list[] = { defines, frag_basic };
	char* vert_basic_list[] = { defines, vert_basic };
	int frag_basic_length_list[] = { defines_length, frag_basic_length };
	int vert_basic_length_list[] = { defines_length, vert_basic_length };
	basic_shaders[0] = create_shader(GL_FRAGMENT_SHADER, 2, (const char**)frag_basic_list, frag_basic_length_list);
	basic_shaders[1] = create_shader(GL_VERTEX_SHADER, 2, (const char**)vert_basic_list, vert_basic_length_list);
	basic_program = create_program(2, basic_shaders);
	glUseProgram(basic_program);
	
	free(vert_basic);
	free(frag_basic);
	
	int world_loc = glGetUniformLocation(basic_program, "uniform_world");
	int view_loc = glGetUniformLocation(basic_program, "uniform_view");
	int proj_loc = glGetUniformLocation(basic_program, "uniform_proj");
	
	begin = get_time_ms();
	counter = 0;
	float delta = 0.0f;
	float timer = 0.0f;
	while (1) {
		
		last = start;
		start = get_time_ms();
		delta = (float)(start - last) * 0.001f;
		
		++counter;
		if (start - begin >= 1000) {
			
			g_fps = counter;
			counter = 0;
			begin = start;
		}
		
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			
			TranslateMessage(&message);
			DispatchMessage(&message);
			if (message.message == WM_QUIT) {
				
				break;
			}
		}
		
		if (message.message == WM_QUIT) {
			
			break;
		}
		
		timer = fmod(timer + delta, M_PI * 2.0f);
		
		float ar = 720.0 / 1280.0;
		float eye[3] = {0.0f, -5.0f, 5.0f};
		float look[3] = {0.0f, 0.0f, 0.5f};
		float up[3] = {0.0f, 0.0f, 1.0f};
		float translation[3] = {sin(timer), 0.0f, 0.0f};
		float scale[3] = {1.0f , 1.0f, 1.0f};
		float rotation[4];
		float world[16], view[16], proj[16];
		
		quat_axis_angle(rotation, up, 0.0f);
		mat4_transformation(world, translation, rotation, scale);
		mat4_lookat(view, eye, look, up);
		mat4_frustum(proj, -1.0f / ar, 1.0f / ar, -1.0f, 1.0f, 1.0f, 10.0f);
		#define ATHINGY mat4_perspective(proj, 1280.0f / 720.0f, M_PI * 0.5f, 0.1f, 10.0f);
		
		glUniformMatrix4fv(world_loc, 1, 0, (const float*)world);
		glUniformMatrix4fv(view_loc, 1, 0, (const float*)view);
		glUniformMatrix4fv(proj_loc, 1, 0, (const float*)proj);
		render();
		
		diff = get_time_ms() - start;
		if (diff <= 17) {
			
			Sleep(17 - diff);
		}
	}
	
	shutdown_renderer();
	shutdown_window();
	
	return 1;
}