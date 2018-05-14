
#include "window_renderer.h"
#include "vector_math.h"
#include <stdio.h>
#include <string.h>

#define VOXEL_RES 8
#define VOXEL_RES2 (VOXEL_RES * VOXEL_RES)
#define VOXEL_RES3 (VOXEL_RES * VOXEL_RES * VOXEL_RES)

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
unsigned int g_voxel_index_count;
float g_noise_buffer[256];

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

float oversample_noise_3d(float* t_vec) {
	
	return (oversample_noise(t_vec[0], t_vec[1]) + oversample_noise(t_vec[1], t_vec[2])) * 0.5f;
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
	glDrawElements(GL_TRIANGLES, g_voxel_index_count, GL_UNSIGNED_INT, 0);
	
	SwapBuffers(g_device);
}

void setup_game() {
	
	static const float lookup_voxel_vertices[12][3] =
		{ { 0.5f,	1.0f,	0.0f	}, { 1.0f,	0.5f,	0.0f	}, { 0.5f,	0.0f,	0.0f	}, { 0.0f,	0.5f,	0.0f	}
		, { 0.5f,	1.0f,	1.0f	}, { 1.0f,	0.5f,	1.0f	}, { 0.5f,	0.0f,	1.0f	}, { 0.0f,	0.5f,	1.0f	}
		, { 0.0f,	1.0f,	0.5f	}, { 1.0f,	1.0f,	0.5f	}, { 1.0f,	0.0f,	0.5f	}, { 0.0f,	0.0f,	0.5f	} };
	static const signed char lookup_voxel_indices[256][16] = 
		{{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,8,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,8,3,9,8,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,8,3,1,2,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,2,10,0,2,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{2,8,3,2,10,8,10,9,8,-1,-1,-1,-1,-1,-1,-1},{3,11,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,11,2,8,11,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,9,0,2,3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,11,2,1,9,11,9,8,11,-1,-1,-1,-1,-1,-1,-1},{3,10,1,11,10,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,10,1,0,8,10,8,11,10,-1,-1,-1,-1,-1,-1,-1},{3,9,0,3,11,9,11,10,9,-1,-1,-1,-1,-1,-1,-1},{9,8,10,10,8,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,7,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,3,0,7,3,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,1,9,8,4,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,1,9,4,7,1,7,3,1,-1,-1,-1,-1,-1,-1,-1},{1,2,10,8,4,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,4,7,3,0,4,1,2,10,-1,-1,-1,-1,-1,-1,-1},{9,2,10,9,0,2,8,4,7,-1,-1,-1,-1,-1,-1,-1},{2,10,9,2,9,7,2,7,3,7,9,4,-1,-1,-1,-1},{8,4,7,3,11,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{11,4,7,11,2,4,2,0,4,-1,-1,-1,-1,-1,-1,-1},{9,0,1,8,4,7,2,3,11,-1,-1,-1,-1,-1,-1,-1},{4,7,11,9,4,11,9,11,2,9,2,1,-1,-1,-1,-1},{3,10,1,3,11,10,7,8,4,-1,-1,-1,-1,-1,-1,-1},{1,11,10,1,4,11,1,0,4,7,11,4,-1,-1,-1,-1},{4,7,8,9,0,11,9,11,10,11,0,3,-1,-1,-1,-1},{4,7,11,4,11,9,9,11,10,-1,-1,-1,-1,-1,-1,-1},{9,5,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,5,4,0,8,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,5,4,1,5,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{8,5,4,8,3,5,3,1,5,-1,-1,-1,-1,-1,-1,-1},{1,2,10,9,5,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,0,8,1,2,10,4,9,5,-1,-1,-1,-1,-1,-1,-1},{5,2,10,5,4,2,4,0,2,-1,-1,-1,-1,-1,-1,-1},{2,10,5,3,2,5,3,5,4,3,4,8,-1,-1,-1,-1},{9,5,4,2,3,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,11,2,0,8,11,4,9,5,-1,-1,-1,-1,-1,-1,-1},{0,5,4,0,1,5,2,3,11,-1,-1,-1,-1,-1,-1,-1},{2,1,5,2,5,8,2,8,11,4,8,5,-1,-1,-1,-1},{10,3,11,10,1,3,9,5,4,-1,-1,-1,-1,-1,-1,-1},{4,9,5,0,8,1,8,10,1,8,11,10,-1,-1,-1,-1},{5,4,0,5,0,11,5,11,10,11,0,3,-1,-1,-1,-1},{5,4,8,5,8,10,10,8,11,-1,-1,-1,-1,-1,-1,-1},{9,7,8,5,7,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,3,0,9,5,3,5,7,3,-1,-1,-1,-1,-1,-1,-1},{0,7,8,0,1,7,1,5,7,-1,-1,-1,-1,-1,-1,-1},{1,5,3,3,5,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,7,8,9,5,7,10,1,2,-1,-1,-1,-1,-1,-1,-1},{10,1,2,9,5,0,5,3,0,5,7,3,-1,-1,-1,-1},{8,0,2,8,2,5,8,5,7,10,5,2,-1,-1,-1,-1},{2,10,5,2,5,3,3,5,7,-1,-1,-1,-1,-1,-1,-1},{7,9,5,7,8,9,3,11,2,-1,-1,-1,-1,-1,-1,-1},{9,5,7,9,7,2,9,2,0,2,7,11,-1,-1,-1,-1},{2,3,11,0,1,8,1,7,8,1,5,7,-1,-1,-1,-1},{11,2,1,11,1,7,7,1,5,-1,-1,-1,-1,-1,-1,-1},{9,5,8,8,5,7,10,1,3,10,3,11,-1,-1,-1,-1},{5,7,0,5,0,9,7,11,0,1,0,10,11,10,0,-1},{11,10,0,11,0,3,10,5,0,8,0,7,5,7,0,-1},{11,10,5,7,11,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{10,6,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,8,3,5,10,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,0,1,5,10,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,8,3,1,9,8,5,10,6,-1,-1,-1,-1,-1,-1,-1},{1,6,5,2,6,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,6,5,1,2,6,3,0,8,-1,-1,-1,-1,-1,-1,-1},{9,6,5,9,0,6,0,2,6,-1,-1,-1,-1,-1,-1,-1},{5,9,8,5,8,2,5,2,6,3,2,8,-1,-1,-1,-1},{2,3,11,10,6,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{11,0,8,11,2,0,10,6,5,-1,-1,-1,-1,-1,-1,-1},{0,1,9,2,3,11,5,10,6,-1,-1,-1,-1,-1,-1,-1},{5,10,6,1,9,2,9,11,2,9,8,11,-1,-1,-1,-1},{6,3,11,6,5,3,5,1,3,-1,-1,-1,-1,-1,-1,-1},{0,8,11,0,11,5,0,5,1,5,11,6,-1,-1,-1,-1},{3,11,6,0,3,6,0,6,5,0,5,9,-1,-1,-1,-1},{6,5,9,6,9,11,11,9,8,-1,-1,-1,-1,-1,-1,-1},{5,10,6,4,7,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,3,0,4,7,3,6,5,10,-1,-1,-1,-1,-1,-1,-1},{1,9,0,5,10,6,8,4,7,-1,-1,-1,-1,-1,-1,-1},{10,6,5,1,9,7,1,7,3,7,9,4,-1,-1,-1,-1},{6,1,2,6,5,1,4,7,8,-1,-1,-1,-1,-1,-1,-1},{1,2,5,5,2,6,3,0,4,3,4,7,-1,-1,-1,-1},{8,4,7,9,0,5,0,6,5,0,2,6,-1,-1,-1,-1},{7,3,9,7,9,4,3,2,9,5,9,6,2,6,9,-1},{3,11,2,7,8,4,10,6,5,-1,-1,-1,-1,-1,-1,-1},{5,10,6,4,7,2,4,2,0,2,7,11,-1,-1,-1,-1},{0,1,9,4,7,8,2,3,11,5,10,6,-1,-1,-1,-1},{9,2,1,9,11,2,9,4,11,7,11,4,5,10,6,-1},{8,4,7,3,11,5,3,5,1,5,11,6,-1,-1,-1,-1},{5,1,11,5,11,6,1,0,11,7,11,4,0,4,11,-1},{0,5,9,0,6,5,0,3,6,11,6,3,8,4,7,-1},{6,5,9,6,9,11,4,7,9,7,11,9,-1,-1,-1,-1},{10,4,9,6,4,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,10,6,4,9,10,0,8,3,-1,-1,-1,-1,-1,-1,-1},{10,0,1,10,6,0,6,4,0,-1,-1,-1,-1,-1,-1,-1},{8,3,1,8,1,6,8,6,4,6,1,10,-1,-1,-1,-1},{1,4,9,1,2,4,2,6,4,-1,-1,-1,-1,-1,-1,-1},{3,0,8,1,2,9,2,4,9,2,6,4,-1,-1,-1,-1},{0,2,4,4,2,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{8,3,2,8,2,4,4,2,6,-1,-1,-1,-1,-1,-1,-1},{10,4,9,10,6,4,11,2,3,-1,-1,-1,-1,-1,-1,-1},{0,8,2,2,8,11,4,9,10,4,10,6,-1,-1,-1,-1},{3,11,2,0,1,6,0,6,4,6,1,10,-1,-1,-1,-1},{6,4,1,6,1,10,4,8,1,2,1,11,8,11,1,-1},{9,6,4,9,3,6,9,1,3,11,6,3,-1,-1,-1,-1},{8,11,1,8,1,0,11,6,1,9,1,4,6,4,1,-1},{3,11,6,3,6,0,0,6,4,-1,-1,-1,-1,-1,-1,-1},{6,4,8,11,6,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{7,10,6,7,8,10,8,9,10,-1,-1,-1,-1,-1,-1,-1},{0,7,3,0,10,7,0,9,10,6,7,10,-1,-1,-1,-1},{10,6,7,1,10,7,1,7,8,1,8,0,-1,-1,-1,-1},{10,6,7,10,7,1,1,7,3,-1,-1,-1,-1,-1,-1,-1},{1,2,6,1,6,8,1,8,9,8,6,7,-1,-1,-1,-1},{2,6,9,2,9,1,6,7,9,0,9,3,7,3,9,-1},{7,8,0,7,0,6,6,0,2,-1,-1,-1,-1,-1,-1,-1},{7,3,2,6,7,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{2,3,11,10,6,8,10,8,9,8,6,7,-1,-1,-1,-1},{2,0,7,2,7,11,0,9,7,6,7,10,9,10,7,-1},{1,8,0,1,7,8,1,10,7,6,7,10,2,3,11,-1},{11,2,1,11,1,7,10,6,1,6,7,1,-1,-1,-1,-1},{8,9,6,8,6,7,9,1,6,11,6,3,1,3,6,-1},{0,9,1,11,6,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{7,8,0,7,0,6,3,11,0,11,6,0,-1,-1,-1,-1},{7,11,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{7,6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,0,8,11,7,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,1,9,11,7,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{8,1,9,8,3,1,11,7,6,-1,-1,-1,-1,-1,-1,-1},{10,1,2,6,11,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,2,10,3,0,8,6,11,7,-1,-1,-1,-1,-1,-1,-1},{2,9,0,2,10,9,6,11,7,-1,-1,-1,-1,-1,-1,-1},{6,11,7,2,10,3,10,8,3,10,9,8,-1,-1,-1,-1},{7,2,3,6,2,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{7,0,8,7,6,0,6,2,0,-1,-1,-1,-1,-1,-1,-1},{2,7,6,2,3,7,0,1,9,-1,-1,-1,-1,-1,-1,-1},{1,6,2,1,8,6,1,9,8,8,7,6,-1,-1,-1,-1},{10,7,6,10,1,7,1,3,7,-1,-1,-1,-1,-1,-1,-1},{10,7,6,1,7,10,1,8,7,1,0,8,-1,-1,-1,-1},{0,3,7,0,7,10,0,10,9,6,10,7,-1,-1,-1,-1},{7,6,10,7,10,8,8,10,9,-1,-1,-1,-1,-1,-1,-1},{6,8,4,11,8,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,6,11,3,0,6,0,4,6,-1,-1,-1,-1,-1,-1,-1},{8,6,11,8,4,6,9,0,1,-1,-1,-1,-1,-1,-1,-1},{9,4,6,9,6,3,9,3,1,11,3,6,-1,-1,-1,-1},{6,8,4,6,11,8,2,10,1,-1,-1,-1,-1,-1,-1,-1},{1,2,10,3,0,11,0,6,11,0,4,6,-1,-1,-1,-1},{4,11,8,4,6,11,0,2,9,2,10,9,-1,-1,-1,-1},{10,9,3,10,3,2,9,4,3,11,3,6,4,6,3,-1},{8,2,3,8,4,2,4,6,2,-1,-1,-1,-1,-1,-1,-1},{0,4,2,4,6,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,9,0,2,3,4,2,4,6,4,3,8,-1,-1,-1,-1},{1,9,4,1,4,2,2,4,6,-1,-1,-1,-1,-1,-1,-1},{8,1,3,8,6,1,8,4,6,6,10,1,-1,-1,-1,-1},{10,1,0,10,0,6,6,0,4,-1,-1,-1,-1,-1,-1,-1},{4,6,3,4,3,8,6,10,3,0,3,9,10,9,3,-1},{10,9,4,6,10,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,9,5,7,6,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,8,3,4,9,5,11,7,6,-1,-1,-1,-1,-1,-1,-1},{5,0,1,5,4,0,7,6,11,-1,-1,-1,-1,-1,-1,-1},{11,7,6,8,3,4,3,5,4,3,1,5,-1,-1,-1,-1},{9,5,4,10,1,2,7,6,11,-1,-1,-1,-1,-1,-1,-1},{6,11,7,1,2,10,0,8,3,4,9,5,-1,-1,-1,-1},{7,6,11,5,4,10,4,2,10,4,0,2,-1,-1,-1,-1},{3,4,8,3,5,4,3,2,5,10,5,2,11,7,6,-1},{7,2,3,7,6,2,5,4,9,-1,-1,-1,-1,-1,-1,-1},{9,5,4,0,8,6,0,6,2,6,8,7,-1,-1,-1,-1},{3,6,2,3,7,6,1,5,0,5,4,0,-1,-1,-1,-1},{6,2,8,6,8,7,2,1,8,4,8,5,1,5,8,-1},{9,5,4,10,1,6,1,7,6,1,3,7,-1,-1,-1,-1},{1,6,10,1,7,6,1,0,7,8,7,0,9,5,4,-1},{4,0,10,4,10,5,0,3,10,6,10,7,3,7,10,-1},{7,6,10,7,10,8,5,4,10,4,8,10,-1,-1,-1,-1},{6,9,5,6,11,9,11,8,9,-1,-1,-1,-1,-1,-1,-1},{3,6,11,0,6,3,0,5,6,0,9,5,-1,-1,-1,-1},{0,11,8,0,5,11,0,1,5,5,6,11,-1,-1,-1,-1},{6,11,3,6,3,5,5,3,1,-1,-1,-1,-1,-1,-1,-1},{1,2,10,9,5,11,9,11,8,11,5,6,-1,-1,-1,-1},{0,11,3,0,6,11,0,9,6,5,6,9,1,2,10,-1},{11,8,5,11,5,6,8,0,5,10,5,2,0,2,5,-1},{6,11,3,6,3,5,2,10,3,10,5,3,-1,-1,-1,-1},{5,8,9,5,2,8,5,6,2,3,8,2,-1,-1,-1,-1},{9,5,6,9,6,0,0,6,2,-1,-1,-1,-1,-1,-1,-1},{1,5,8,1,8,0,5,6,8,3,8,2,6,2,8,-1},{1,5,6,2,1,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,3,6,1,6,10,3,8,6,5,6,9,8,9,6,-1},{10,1,0,10,0,6,9,5,0,5,6,0,-1,-1,-1,-1},{0,3,8,5,6,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{10,5,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{11,5,10,7,5,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{11,5,10,11,7,5,8,3,0,-1,-1,-1,-1,-1,-1,-1},{5,11,7,5,10,11,1,9,0,-1,-1,-1,-1,-1,-1,-1},{10,7,5,10,11,7,9,8,1,8,3,1,-1,-1,-1,-1},{11,1,2,11,7,1,7,5,1,-1,-1,-1,-1,-1,-1,-1},{0,8,3,1,2,7,1,7,5,7,2,11,-1,-1,-1,-1},{9,7,5,9,2,7,9,0,2,2,11,7,-1,-1,-1,-1},{7,5,2,7,2,11,5,9,2,3,2,8,9,8,2,-1},{2,5,10,2,3,5,3,7,5,-1,-1,-1,-1,-1,-1,-1},{8,2,0,8,5,2,8,7,5,10,2,5,-1,-1,-1,-1},{9,0,1,5,10,3,5,3,7,3,10,2,-1,-1,-1,-1},{9,8,2,9,2,1,8,7,2,10,2,5,7,5,2,-1},{1,3,5,3,7,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,8,7,0,7,1,1,7,5,-1,-1,-1,-1,-1,-1,-1},{9,0,3,9,3,5,5,3,7,-1,-1,-1,-1,-1,-1,-1},{9,8,7,5,9,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{5,8,4,5,10,8,10,11,8,-1,-1,-1,-1,-1,-1,-1},{5,0,4,5,11,0,5,10,11,11,3,0,-1,-1,-1,-1},{0,1,9,8,4,10,8,10,11,10,4,5,-1,-1,-1,-1},{10,11,4,10,4,5,11,3,4,9,4,1,3,1,4,-1},{2,5,1,2,8,5,2,11,8,4,5,8,-1,-1,-1,-1},{0,4,11,0,11,3,4,5,11,2,11,1,5,1,11,-1},{0,2,5,0,5,9,2,11,5,4,5,8,11,8,5,-1},{9,4,5,2,11,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{2,5,10,3,5,2,3,4,5,3,8,4,-1,-1,-1,-1},{5,10,2,5,2,4,4,2,0,-1,-1,-1,-1,-1,-1,-1},{3,10,2,3,5,10,3,8,5,4,5,8,0,1,9,-1},{5,10,2,5,2,4,1,9,2,9,4,2,-1,-1,-1,-1},{8,4,5,8,5,3,3,5,1,-1,-1,-1,-1,-1,-1,-1},{0,4,5,1,0,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{8,4,5,8,5,3,9,0,5,0,3,5,-1,-1,-1,-1},{9,4,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,11,7,4,9,11,9,10,11,-1,-1,-1,-1,-1,-1,-1},{0,8,3,4,9,7,9,11,7,9,10,11,-1,-1,-1,-1},{1,10,11,1,11,4,1,4,0,7,4,11,-1,-1,-1,-1},{3,1,4,3,4,8,1,10,4,7,4,11,10,11,4,-1},{4,11,7,9,11,4,9,2,11,9,1,2,-1,-1,-1,-1},{9,7,4,9,11,7,9,1,11,2,11,1,0,8,3,-1},{11,7,4,11,4,2,2,4,0,-1,-1,-1,-1,-1,-1,-1},{11,7,4,11,4,2,8,3,4,3,2,4,-1,-1,-1,-1},{2,9,10,2,7,9,2,3,7,7,4,9,-1,-1,-1,-1},{9,10,7,9,7,4,10,2,7,8,7,0,2,0,7,-1},{3,7,10,3,10,2,7,4,10,1,10,0,4,0,10,-1},{1,10,2,8,7,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,9,1,4,1,7,7,1,3,-1,-1,-1,-1,-1,-1,-1},{4,9,1,4,1,7,0,8,1,8,7,1,-1,-1,-1,-1},{4,0,3,7,4,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{4,8,7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{9,10,8,10,11,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,0,9,3,9,11,11,9,10,-1,-1,-1,-1,-1,-1,-1},{0,1,10,0,10,8,8,10,11,-1,-1,-1,-1,-1,-1,-1},{3,1,10,11,3,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,2,11,1,11,9,9,11,8,-1,-1,-1,-1,-1,-1,-1},{3,0,9,3,9,11,1,2,9,2,11,9,-1,-1,-1,-1},{0,2,11,8,0,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{3,2,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{2,3,8,2,8,10,10,8,9,-1,-1,-1,-1,-1,-1,-1},{9,10,2,0,9,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{2,3,8,2,8,10,0,1,8,1,10,8,-1,-1,-1,-1},{1,10,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{1,3,8,9,1,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,9,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,3,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};
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

	float voxel_vertices[VOXEL_RES3 * 3 * 16];
	unsigned int voxel_indices[VOXEL_RES3 * 16];
	printf_white("SIZE OF: %u", (unsigned int)(sizeof(voxel_vertices) / 3));
	float off[3], tmp[3];
	unsigned char index;
	for (i = 0; i < VOXEL_RES3; ++i) {
		
		off[0] = (float)(i % VOXEL_RES);
		off[1] = (float)((i / VOXEL_RES) % VOXEL_RES);
		off[2] = (float)((i / VOXEL_RES2) % VOXEL_RES);
		index = 0;
		tmp[0] = 0.0f;	tmp[1] = 1.0f;	tmp[2] = 0.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 0;
		tmp[0] = 1.0f;	tmp[1] = 1.0f;	tmp[2] = 0.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 1;
		tmp[0] = 1.0f;	tmp[1] = 0.0f;	tmp[2] = 0.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 2;
		tmp[0] = 0.0f;	tmp[1] = 0.0f;	tmp[2] = 0.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 3;
		tmp[0] = 0.0f;	tmp[1] = 1.0f;	tmp[2] = 1.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 4;
		tmp[0] = 1.0f;	tmp[1] = 1.0f;	tmp[2] = 1.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 5;
		tmp[0] = 1.0f;	tmp[1] = 0.0f;	tmp[2] = 1.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 6;
		tmp[0] = 0.0f;	tmp[1] = 0.0f;	tmp[2] = 1.0f;
		index |= (oversample_noise_3d(vec3_muls(tmp, vec3_add(tmp, off, tmp), 4.0f)) < 0.5f) << 7;
		
		for (j = 0; j < 12; ++j) {
			
			signed char id = lookup_voxel_indices[index][j];
			if (id < 0) {
				
				break;
			}
			vec3_add(&voxel_vertices[g_voxel_index_count * 3], lookup_voxel_vertices[id], off);
			voxel_indices[g_voxel_index_count] = g_voxel_index_count;
			++g_voxel_index_count;
		}
	}
	
	
	glGenVertexArrays(1, &g_voxel_buffers[0]);
	glBindVertexArray(g_voxel_buffers[0]);
	glGenBuffers(2, &g_voxel_buffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, g_voxel_buffers[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_voxel_buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, g_voxel_index_count * 3 * sizeof(float), voxel_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_voxel_index_count * sizeof(unsigned int), voxel_indices, GL_STATIC_DRAW);
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
	
	printf_white("SIZE_MAX: %u", (unsigned int)(SIZE_MAX));
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