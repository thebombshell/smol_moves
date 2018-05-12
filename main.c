
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
unsigned int g_voxel_buffers[3];
unsigned int g_voxel_count;
float g_noise_buffer[256];

#define VOXEL_RES 8
#define VOXEL_RES2 (VOXEL_RES * VOXEL_RES)
#define VOXEL_RES3 (VOXEL_RES * VOXEL_RES * VOXEL_RES)

float sample_noise(float t_x, float t_y) {
	
	float floor_x = floor(t_x);
	float floor_y = floor(t_y);
	float alpha_x = t_x - floor_x;
	float alpha_y = t_y - floor_y;
	float ialpha_x = 1.0f - alpha_x;
	unsigned int l = (unsigned int)floor_x % 16;
	unsigned int t = (unsigned int)floor_y % 16;
	unsigned int r = l == 15 ? 0 : (l + 1);
	unsigned int b = t == 15 ? 0 : (t + 1);
	
	return (g_noise_buffer[l * 16 + t] * ialpha_x + g_noise_buffer[r * 16 + t] * alpha_x) * (1.0f - alpha_y) 
		+ (g_noise_buffer[l * 16 + b] * ialpha_x + g_noise_buffer[r * 16 + b] * alpha_x) * alpha_y;
}

float oversample_noise(float t_x, float t_y) {
	
	return (sample_noise(t_x, t_y) + sample_noise(t_x * 2.0f, t_y * 2.0f) + sample_noise(t_x * 4.0f, t_y * 4.0f)) * 0.3333f;
}

float oversample_noise_3d(float t_x, float t_y, float t_z) {
	
	return (oversample_noise(t_x, t_y) + oversample_noise(t_y, t_z)) * 0.5f;
}

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
	mat4_perspective(proj, ar, M_PI * 0.5f, 0.1f, 100.0f);
	
	glUniformMatrix4fv(g_world_loc, 1, 0, (const float*)world);
	glUniformMatrix4fv(g_view_loc, 1, 0, (const float*)view);
	glUniformMatrix4fv(g_proj_loc, 1, 0, (const float*)proj);
		
	glBindVertexArray(g_cube_buffers[0]);
	glDrawElements(GL_TRIANGLES, geometry_cube_get_index_count(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(g_voxel_buffers[0]);
	glDrawElements(GL_TRIANGLES, g_voxel_count * 12, GL_UNSIGNED_INT, 0);
	
	SwapBuffers(g_device);
}

void setup_game() {
	
	static const float lookup_voxel_vertices[36] =
		{ -0.5f, 0.0f, 0.5f
		, 0.0f, 0.5f, 0.5f
		, 0.5f, 0.0f, 0.5f
		, 0.0f, -0.5f, 0.5f
		, -0.5f, 0.5f, 0.0f
		, 0.5f, 0.5f, 0.0f
		, 0.5f, -0.5f, 0.0f
		, -0.5f, -0.5f, 0.0f
		, -0.5f, 0.0f, -0.5f
		, 0.0f, 0.5f, -0.5f
		, 0.5f, 0.0f, -0.5f
		, 0.0f, -0.5f, -0.5f };
	static const unsigned int lookup_voxel_indices[256][12] = 
		{	{ 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LBN_ 1
		,	{ 7, 11, 8
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBN_ 2
		,	{ 6, 10, 11
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBN_LBN 3
		,	{ 7, 6, 10
			, 10, 8, 7
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBF_ 4
		,	{ 5, 9, 10
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBF_LBN 5
		,	{ 5, 9, 10
			, 7, 11, 8
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBF_RBN 6
		,	{ 6, 5, 9
			, 9, 11, 6
			, 0, 0, 0
			, 0, 0, 0}
			
#		define RBF_RBN_LBN 7
		,	{ 7, 6, 5
			, 7, 5, 9
			, 9, 8, 7
			, 0, 0, 0}
			
#		define LBF_ 8
		,	{ 4, 8, 6
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LBF_LBN 9
		,	{ 4, 7, 11
			, 11, 9, 4
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LBF_RBN 10
		,	{ 4, 8, 6
			, 6, 10, 11
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LBF_RBN_LBN 11
		,	{ 4, 7, 6
			, 6, 10, 9
			, 9, 4, 6
			, 0, 0, 0}
			
#		define LBF_RBF 12
		,	{ 5, 4, 8
			, 8, 10, 5
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LBF_RBF_LBN 13
		,	{ 5, 4, 7
			, 7, 11, 10
			, 10, 5, 7
			, 0, 0, 0}
			
#		define LBF_RBF_RBN 14
		,	{ 6, 5, 4
			, 4, 8, 11
			, 11, 6, 4
			, 0, 0, 0}
			
#		define LBF_RBF_RBN_LBN 15
		,	{ 5, 4, 7
			, 7, 6, 5
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LTN_ 16
		,	{ 7, 0, 3
			, 0, 0, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LTN_LBN_ 17
		,	{ 0, 3, 11
			, 11, 8, 0
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LTN_RBN_ 18
		,	{ 7, 0, 3
			, 6, 10, 11
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LTN_RBN_LBN 19
		,	{ 0, 10, 8
			, 0, 3, 10
			, 10, 3, 6
			, 0, 0, 0}
			
#		define LTN_RBN_LBN 19
		,	{ 7, 0, 3
			, 5, 9, 10
			, 0, 0, 0
			, 0, 0, 0}
			
#		define LTN_RBF_ 20
		,	{ 0, 3, 11
			, 11, 8, 0
			, 5, 9, 10
			, 0, 0, 0}
			
#		define LTN_RBF_LBN 21
			
		,	{ 7, 0, 3
			, 6, 5, 9
			, 9, 11, 6
			, 0, 0, 0}
			
#		define LTN_RBF_RBN 22

#		define LTN_RBF_RBN_LBN 23
			
#		define LTN_LBF_ 24
			
#		define LTN_LBF_LBN 25
			
#		define LTN_LBF_RBN 26
			
#		define LTN_LBF_RBN_LBN 27
			
#		define LTN_LBF_RBF 28
			
#		define LTN_LBF_RBF_LBN 29
			
#		define LTN_LBF_RBF_RBN 30
			
#		define LTN_LBF_RBF_RBN_LBN 31

#		define RTN_ 32

#		define RTF_ 64

#		define LTF_ 128
		};
	static const char* shader_paths[2] = { "shaders/vert_basic.glsl", "shaders/frag_basic.glsl" };
	static const unsigned int shader_types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	static const char* header = "#version 330\n";
	static unsigned int i, j;
	
	srand(get_time_ms());
	for (i = 0; i < 256; ++i) {
		
		g_noise_buffer[i] = randf();
	}
	
	g_program = create_program_from_files(2, shader_paths, shader_types, header, strlen(header));
	g_world_loc = glGetUniformLocation(g_program, "uniform_world");
	g_view_loc = glGetUniformLocation(g_program, "uniform_view");
	g_proj_loc = glGetUniformLocation(g_program, "uniform_proj");
	glUseProgram(g_program);

	float voxel_vertices[VOXEL_RES3 * 36];
	unsigned int voxel_indices[VOXEL_RES3 * 12];
	
	float x, y, z;
	unsigned int index;
	for (i = 0; i < VOXEL_RES3; ++i) {
		
		x = (float)(i % VOXEL_RES);
		y = (float)((i / VOXEL_RES) % VOXEL_RES);
		z = (float)((i / VOXEL_RES2) % VOXEL_RES);
		index = 0;
		index |= oversample_noise_3d(x - 0.5f, y - 0.5f, z - 0.5f) < 0.5f ? 0 : (1 << 0);
		index |= oversample_noise_3d(x + 0.5f, y - 0.5f, z - 0.5f) < 0.5f ? 0 : (1 << 1);
		index |= oversample_noise_3d(x + 0.5f, y + 0.5f, z - 0.5f) < 0.5f ? 0 : (1 << 2);
		index |= oversample_noise_3d(x - 0.5f, y + 0.5f, z - 0.5f) < 0.5f ? 0 : (1 << 3);
		index |= oversample_noise_3d(x - 0.5f, y - 0.5f, z + 0.5f) < 0.5f ? 0 : (1 << 4);
		index |= oversample_noise_3d(x + 0.5f, y - 0.5f, z + 0.5f) < 0.5f ? 0 : (1 << 5);
		index |= oversample_noise_3d(x + 0.5f, y + 0.5f, z + 0.5f) < 0.5f ? 0 : (1 << 6);
		index |= oversample_noise_3d(x - 0.5f, y + 0.5f, z + 0.5f) < 0.5f ? 0 : (1 << 7);
		
		if (index == 0 || index == 255 || index > 22) {
			
			continue;
		}
		
		memcpy(&voxel_vertices[g_voxel_count * 36], lookup_voxel_vertices, sizeof(float) * 36);
		memcpy(&voxel_indices[g_voxel_count * 12], lookup_voxel_indices[index], sizeof(unsigned int) * 12);
		for (j = 0; j < 12; ++j) {
			
			voxel_vertices[g_voxel_count * 36 + j * 3 + 0] += x;
			voxel_vertices[g_voxel_count * 36 + j * 3 + 1] += y;
			voxel_vertices[g_voxel_count * 36 + j * 3 + 2] += z;
			voxel_indices[g_voxel_count * 12 + j] += g_voxel_count * 12;
		}
		++g_voxel_count;
	}
	
	
	glGenVertexArrays(1, &g_voxel_buffers[0]);
	glBindVertexArray(g_voxel_buffers[0]);
	glGenBuffers(2, &g_voxel_buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, g_voxel_buffers[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_voxel_buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, g_voxel_count * 36 * sizeof(float), voxel_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_voxel_count * 12 * sizeof(unsigned int), voxel_indices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), 0);
	glBindVertexArray(0);
	
	
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