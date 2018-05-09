
#include "window_renderer.h"
#include "vector_math.h"
#include <stdio.h>
#include <string.h>

unsigned long g_fps;

unsigned int g_program;
unsigned int g_world_loc;
unsigned int g_view_loc;
unsigned int g_proj_loc;

float g_camera_eye[3] = {0.0f, -1.0f, 0.0f};
float g_camera_look[3] = {0.0f, 0.0f, 0.0f};
float g_camera_up[3] = {0.0f, 0.0f, 1.0f};
float g_look_x = M_PI * 0.5f;
float g_look_y = M_PI * 0.5f;

unsigned int g_cube_buffers[3];


void update(float t_delta) {
	
	g_look_x += (float)(g_is_key_down[VK_LEFT] - g_is_key_down[VK_RIGHT]) * t_delta;
	g_look_y += (float)(g_is_key_down[VK_DOWN] - g_is_key_down[VK_UP]) * t_delta;
	g_look_y = g_look_y < 0.01f ? 0.01f : (g_look_y > M_PI - 0.01f ? M_PI - 0.01f : g_look_y);
	
	float e = sin(g_look_y);
	e = e < 0.0f ? -e : e;
	float x = cos(g_look_x) * e;
	float y = sin(g_look_x) * e;
	float z = cos(g_look_y);
	
	float up[3] = {0.0f, 0.0f, 1.0f};
	float forward[3] = {x, y, z};
	float right[3];
	float temp[3] = {x, y, z};
	
	vec3_normalize(right, vec3_cross(right, forward, up));
	
	vec3_muls(forward, forward, t_delta * (float)(g_is_key_down['W'] - g_is_key_down['S']));
	vec3_muls(right, right, t_delta * (float)(g_is_key_down['D'] - g_is_key_down['A']));
	vec3_muls(up, up, t_delta * (float)(g_is_key_down['E'] - g_is_key_down['Q']));
	
	vec3_add(g_camera_eye, g_camera_eye, forward);
	vec3_add(g_camera_eye, g_camera_eye, right);
	vec3_add(g_camera_eye, g_camera_eye, up);
	
	vec3_add(g_camera_look, g_camera_eye, temp);
}

void render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	float world[16], view[16], proj[16];
	
	mat4_identity(world);
	mat4_lookat(view, g_camera_eye, g_camera_look, g_camera_up);

	float ar = 16.0f / 9.0f;
	mat4_perspective(proj, ar, M_PI * 0.5f, 0.1f, 10.0f);
	
	glUniformMatrix4fv(g_world_loc, 1, 0, (const float*)world);
	glUniformMatrix4fv(g_view_loc, 1, 0, (const float*)view);
	glUniformMatrix4fv(g_proj_loc, 1, 0, (const float*)proj);
		
	glBindVertexArray(g_cube_buffers[0]);
	glDrawElements(GL_TRIANGLES, geometry_cube_get_index_count(), GL_UNSIGNED_INT, 0);
	
	SwapBuffers(g_device);
}

void setup_game() {
	
	static const char* shader_paths[2] = { "shaders/vert_basic.glsl", "shaders/frag_basic.glsl" };
	static const unsigned int shader_types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	static const char* header = "#version 330";
	
	g_program = create_program_from_files(2, shader_paths, shader_types, header, strlen(header));
	g_world_loc = glGetUniformLocation(g_program, "uniform_world");
	g_view_loc = glGetUniformLocation(g_program, "uniform_view");
	g_proj_loc = glGetUniformLocation(g_program, "uniform_proj");
	glUseProgram(g_program);
	
	glGenVertexArrays(1, &g_cube_buffers[0]);
	glBindVertexArray(g_cube_buffers[0]);
	glGenBuffers(2, &g_cube_buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, g_cube_buffers[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_cube_buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, geometry_cube_get_vertex_count() * 3 * sizeof(float), geometry_cube_get_vertices(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry_cube_get_index_count() * sizeof(unsigned int), geometry_cube_get_indices(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), 0);
	glBindVertexArray(0);
	
}

int main(void) {
	
	unsigned long counter = 0;
	float delta = 0.0f;
	unsigned long long diff = 0;
	unsigned long long end_frame = 0;
	MSG message;
	unsigned long long start_frame = 0;
	unsigned long long start_second = 0;
	
	setup_window();
	setup_game();
	
	start_second = get_time_ms();
	while (1) {
		
		end_frame = start_frame;
		start_frame = get_time_ms();
		delta = (float)(start_frame - end_frame) * 0.001f;
		
		++counter;
		if (start_frame - start_second >= 1000) {
			
			g_fps = counter;
			counter = 0;
			start_second = start_frame;
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
		
		update(delta);
		render();
		
		diff = get_time_ms() - start_frame;
		if (diff <= 17) {
			
			Sleep(17 - diff);
		}
	}
	
	shutdown_window();
	
	return 1;
}