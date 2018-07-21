
#include "smol.h"

#include "log.h"

#include <assert.h>
#include <string.h>

/* platform check */

#if		defined _WIN32

#include "windows/smol.inl"

#else

char platform_unsupported[-1];

#endif

/* smol_uniform_object */

smol_uniform_object* smol_alloc_uniform() {
	
	smol_uniform_object* output = malloc(sizeof(smol_uniform_object));
	output->count = 0;
	output->size = 0;
	output->names = malloc(0);
	output->types = malloc(0);
	output->values = malloc(0);
	return output;
}

void smol_free_uniform(smol_uniform_object* t_uniform) {
	
	assert(t_uniform);
	
	free(t_uniform->names);
	free(t_uniform->types);
	free(t_uniform->values);
	free(t_uniform);
}

void smol_uniform_resize(smol_uniform_object* t_uniform, unsigned int t_size) {
	
	assert(t_uniform && t_size < t_uniform->count);
	
	void* temp;
	
	t_uniform->size = 1;
	
	temp = realloc(t_uniform->names, sizeof(char*) * t_uniform->size);
	assert(temp);
	t_uniform->names = temp;
	
	temp = realloc(t_uniform->types, sizeof(unsigned int) * t_uniform->size);
	assert(temp);
	t_uniform->types = temp;
	
	temp = realloc(t_uniform->values, sizeof(void*) * t_uniform->size);
	assert(temp);
	t_uniform->values = temp;
}

void smol_uniform_bind(smol_uniform_object* t_uniform, smol_pass* t_pass) {
	
	assert(t_uniform && t_pass);
	
	unsigned int i, j;
	for (i = 0; i < t_uniform->count; ++i) {
		
		for (j = 0; j < smol_pass_get_uniform_count(t_pass); ++j) {
			
			if (strcmp(t_uniform->names[i], smol_pass_get_uniform(t_pass, j))) {
				
				smol_pass_set_uniform(t_pass, j, 1, t_uniform->values[i]);
				return;
			}
		}
	}
}

/* smol_pass */

smol_pass* smol_alloc_pass(const char* t_vertex_shader, const char* t_fragment_shader) {
	
	assert(t_vertex_shader && t_fragment_shader);
	
	smol_pass* output;
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_shader;
	int result;
	int length;
	unsigned int i;
	char* string;
	
	glShaderSource(vertex_shader, 1, &t_vertex_shader, (void*)strlen(t_vertex_shader));
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &result);
		string = malloc(result);
		glGetShaderInfoLog(vertex_shader, result, &result, string);
		WRN("failed to compile vertex shader: %s", string);
		glDeleteShader(vertex_shader);
		free(string);
		return 0;
	}
	
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &t_vertex_shader, (void*)strlen(t_vertex_shader));
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &result);
		string = malloc(result);
		glGetShaderInfoLog(fragment_shader, result, &result, string);
		WRN("failed to compile fragment shader: %s", string);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		free(string);
		return 0;
	}
	
	output = malloc(sizeof(smol_pass));
	output->name = glCreateProgram();
	glAttachShader(output->name, vertex_shader);
	glAttachShader(output->name, fragment_shader);
	glLinkProgram(output->name);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glGetProgramiv(output->name, GL_LINK_STATUS, &result);
	if (!result) {
		
		glGetProgramiv(output->name, GL_INFO_LOG_LENGTH, &result);
		string = malloc(result);
		glGetProgramInfoLog(output->name, result, &result, string);
		WRN("failed to link shader program: %s", string);
		glDeleteProgram(output->name);
		free(output);
		return 0;
	}
	
	glGetProgramiv(output->name, GL_ACTIVE_UNIFORMS, &output->count);
	assert(output->count >= 0);
	
	output->uniforms = malloc(sizeof(char*) * output->count);
	output->types = malloc(sizeof(unsigned int) * output->count);
	output->locations = malloc(sizeof(unsigned int) * output->count);
	output->sizes = malloc(sizeof(int) * output->count);
	for (i = 0; i < result; ++i) {
		
		glGetActiveUniform(output->name, i, 0, &length, &output->sizes[i], &output->types[i], 0);
		assert(length >= 0);
		
		output->uniforms[i] = malloc(length);
		assert(output->uniforms[i]);
		glGetActiveUniform(output->name, i, length, &length, &output->sizes[i], &output->types[i], output->uniforms[i]);
		output->locations[i] = glGetUniformLocation(output->name, output->uniforms[i]);
	}
	
	return output;
}

void smol_free_pass(smol_pass* t_pass) {
	
	glDeleteProgram(t_pass->name);
	
	free(t_pass->uniforms);
	free(t_pass->types);
	free(t_pass->locations);
	free(t_pass->sizes);
	free(t_pass);
}

void smol_pass_bind(smol_pass* t_pass) {
	
	assert(t_pass);
	
	glUseProgram(t_pass->name);
}

unsigned int smol_pass_get_uniform_count(smol_pass* t_pass) {
	
	assert(t_pass);
	
	return t_pass->count;
}

const char* smol_pass_get_uniform(smol_pass* t_pass, unsigned int t_index) {
	
	assert(t_pass && t_index < t_pass->count);
	
	return t_pass->uniforms[t_index];
}

void smol_pass_set_uniform(smol_pass* t_pass, unsigned int t_index, unsigned int t_count, void* t_value) {
	
	assert(t_pass && t_index < t_pass->count && t_value);
	
	switch (t_pass->types[t_index]) {
		
		case GL_FLOAT:
			
			if (t_count == 1) {
				
				glUniform1f(t_pass->locations[t_index], *((float*)t_value));
			}
			else {
				
				glUniform1fv(t_pass->locations[t_index], t_count, t_value);
			}
			break;
		case GL_FLOAT_VEC2:
			
			glUniform2fv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_FLOAT_VEC3:
			
			glUniform3fv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_FLOAT_VEC4:
			
			glUniform4fv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		case GL_BOOL:
		case GL_INT:
			
			if (t_count == 1) {
				
				glUniform1i(t_pass->locations[t_index], *((int*)t_value));
			}
			else {
				
				glUniform1iv(t_pass->locations[t_index], t_count, t_value);
			}
			break;
		case GL_BOOL_VEC2:
		case GL_INT_VEC2:
			
			glUniform2iv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_BOOL_VEC3:
		case GL_INT_VEC3:
			
			glUniform3iv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_BOOL_VEC4:
		case GL_INT_VEC4:
			
			glUniform4iv(t_pass->locations[t_index], t_count, t_value);
			break;
		case GL_FLOAT_MAT2:
			
			glUniformMatrix2fv(t_pass->locations[t_index], t_count, GL_FALSE, t_value);
			break;
		case GL_FLOAT_MAT3:
			
			glUniformMatrix3fv(t_pass->locations[t_index], t_count, GL_FALSE, t_value);
			break;
		case GL_FLOAT_MAT4:
			
			glUniformMatrix4fv(t_pass->locations[t_index], t_count, GL_FALSE, t_value);
			break;
	}
}

/* smol_stage */

smol_stage* smol_alloc_stage(unsigned int t_pass_count, smol_pass** t_passes) {
	
	assert(t_passes);
	
	smol_stage* output = malloc(sizeof(smol_stage));
	output->pass_count = t_pass_count;
	output->passes = t_passes;
	
	return output;
}

void smol_free_stage(smol_stage* t_stage) {
	
	free(t_stage);
}

unsigned int smol_stage_get_pass_count(smol_stage* t_stage) {
	
	assert(t_stage);
	
	return t_stage->pass_count;
}

smol_pass* smol_stage_get_pass(smol_stage* t_stage, unsigned int t_index) {
	
	assert(t_stage && t_index < t_stage->pass_count);
	
	return t_stage->passes[t_index];
}

/* smol_path */

smol_path* smol_alloc_path(unsigned int t_pass_count, smol_pass** t_passes) {
	
	assert(t_passes);
	
	smol_path* output = malloc(sizeof(smol_path));
	output->pass_count = t_pass_count;
	output->passes = t_passes;
	
	return output;
}

void smol_free_path(smol_path* t_path) {
	
	assert(t_path);
	
	free(t_path);
}

unsigned int smol_path_get_pass_count(smol_path* t_path) {
	
	assert(t_path);
	
	return t_path->pass_count;
}

smol_pass* smol_path_get_pass(smol_path* t_path, unsigned int t_index) {
	
	assert(t_path && t_index < t_path->pass_count);
	
	return t_path->passes[t_index];
}

/* smol_material */

smol_material* smol_alloc_materail(smol_path* t_path, smol_uniform_object* t_uniforms) {
	
	assert(t_path && t_uniforms);
	
	smol_material* output = malloc(sizeof(smol_material));
	output->path = t_path;
	output->uniforms = t_uniforms;
	
	return output;
}

void smol_free_material(smol_material* t_material) {
	
	assert(t_material);
	
	free(t_material);
}

smol_path* smol_material_get_path(smol_material* t_material) {
	
	assert(t_material);
	
	return t_material->path;
}

smol_uniform_object* smol_material_get_uniform_object(smol_material* t_material) {
	
	assert(t_material);
	
	return t_material->uniforms;
}

/* smol_mesh */

smol_mesh* smol_alloc_mesh() {
	
	smol_mesh* output = malloc(sizeof(smol_mesh));
	
	glGenBuffers(2, &output->names[0]);
	output->draw_type = GL_TRIANGLES;
	output->element_count = 0;
	
	return output;
}

void smol_free_mesh(smol_mesh* t_mesh) {
	
	assert(t_mesh);
	
	glDeleteBuffers(2, &t_mesh->names[0]);
	
	free(t_mesh);
}

void smol_mesh_bind(smol_mesh* t_mesh) {
	
	assert(t_mesh);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_mesh->names[0]);
	glBindBuffer(GL_ARRAY_BUFFER, t_mesh->names[1]);
}

void smol_mesh_draw(smol_mesh* t_mesh) {
	
	assert(t_mesh);
	
	glDrawElements(t_mesh->draw_type, t_mesh->element_count, GL_UNSIGNED_SHORT, (void*)0);
}

/* smol_draw */

smol_draw* smol_alloc_draw(smol_mesh* t_mesh, smol_material* t_material, smol_uniform_object* t_uniforms) {
	
	assert(t_mesh && t_material && t_uniforms);
	
	smol_draw* output = malloc(sizeof(smol_draw));
	
	output->mesh = t_mesh;
	output->material = t_material;
	output->uniforms = t_uniforms;
	output->next = 0;
	
	return output;
}

void smol_free_draw(smol_draw* t_draw) {
	
	assert(t_draw);
	
	free(t_draw);
}

smol_uniform_object* smol_draw_get_uniforms(smol_draw* t_draw) {
	
	assert(t_draw);
	
	return t_draw->uniforms;
}

smol_material* smol_draw_get_material(smol_draw* t_draw) {
	
	assert(t_draw);
	
	return t_draw->material;
}

smol_mesh* smol_draw_get_mesh(smol_draw* t_draw) {
	
	assert(t_draw);
	
	return t_draw->mesh;
}

/* smol_view */

smol_view* smol_alloc_view(unsigned int t_stage_count, smol_stage** t_stages, smol_uniform_object* t_uniforms) {
	
	assert(t_stages && t_uniforms);
	
	smol_view* output = malloc(sizeof(smol_view));
	
	output->stage_count = t_stage_count;
	output->stages = t_stages;
	output->uniforms = t_uniforms;
	
	return output;
}

void smol_free_view(smol_view* t_view) {
	
	assert(t_view);
	
	free(t_view);
}

unsigned int smol_scene_get_view_count(smol_scene* t_scene) {
	
	assert(t_scene);
	
	return t_scene->view_count;
}

smol_view* smol_scene_get_view(smol_scene* t_scene, unsigned int t_index) {
	
	assert(t_scene && t_index < t_scene->view_count);
	
	return t_scene->views[t_index];
}

unsigned int smol_view_get_stage_count(smol_view* t_view) {
	
	assert(t_view);
	
	return t_view->stage_count;
}

smol_stage* smol_view_get_stage(smol_view* t_view, unsigned int t_index) {
	
	assert(t_view && t_index < t_view->stage_count);
	
	return t_view->stages[t_index];
}

smol_uniform_object* smol_view_get_uniforms(smol_view* t_view) {
	
	assert(t_view);
	
	return t_view->uniforms;
}

void smol_view_query_visible_draws(smol_view* t_view, smol_scene* t_scene, unsigned int* t_count_out, smol_draw** t_draws_out) {
	
	assert(t_view && t_count_out && t_draws_out);
	
	*t_count_out = smol_scene_get_draw_count(t_scene);
	*t_draws_out = smol_scene_get_draw_head(t_scene);
}

/* smol_scene */

void smol_init_scene(smol_scene* t_scene) {
	
	assert(t_scene);
	
	t_scene->draw_count = 0;
	t_scene->draw_head.mesh = 0;
	t_scene->draw_head.material = 0;
	t_scene->draw_head.uniforms = 0;
	t_scene->draw_head.next = 0;
	t_scene->draw_tail = &t_scene->draw_head;
	t_scene->view_count = 0;
	t_scene->views = malloc(0);
}

void smol_final_scene(smol_scene* t_scene) {
	
	assert(t_scene);
	
	smol_draw* draw = &t_scene->draw_head;
	smol_draw* next = draw->next;
	
	while (next) {
		
		draw = next;
		next = draw->next;
		
		smol_free_draw(draw);
	}
	
	free(t_scene->views);
}

unsigned int smol_scene_get_draw_count(smol_scene* t_scene) {
	
	assert(t_scene);
	
	return t_scene->draw_count;
}

smol_draw* smol_scene_get_draw_head(smol_scene* t_scene) {
	
	assert(t_scene);
	
	return &t_scene->draw_head;
}

void smol_render(smol* t_smol) {
	
	assert(t_smol);
	
	unsigned int i, j, k, l;
	smol_view* view;
	smol_stage* stage;
	smol_pass* pass;
	unsigned int draw_count;
	smol_draw* draw;
	smol_material* material;
	smol_path* path;
	smol_mesh* mesh;
	smol_scene* scene = smol_get_scene(t_smol);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (i = 0; i < smol_scene_get_view_count(scene); ++i) {
		
		view = smol_scene_get_view(scene, i);
		smol_view_query_visible_draws(view, scene, &draw_count, &draw);
		
		for (j = 0; j < smol_view_get_stage_count(view); ++j) {
			
			stage = smol_view_get_stage(view, j);
			
			for (k = 0; k < smol_stage_get_pass_count(stage); ++k) {
				
				pass = smol_stage_get_pass(stage, k);
				smol_pass_bind(pass);
				smol_uniform_bind(smol_view_get_uniforms(view), pass);
				
				while (draw->next) {
					
					draw = draw->next;
					material = smol_draw_get_material(draw);
					path = smol_material_get_path(material);
					for (l = 0; l < smol_path_get_pass_count(path); ++l) {
						
						if (pass == smol_path_get_pass(path, l)) {
							
							smol_uniform_bind(smol_material_get_uniform_object(material), pass);
							smol_uniform_bind(smol_draw_get_uniforms(draw), pass);
							mesh = smol_draw_get_mesh(draw);
							smol_mesh_bind(mesh);
							smol_mesh_draw(mesh);
							break;
						}
					}
				}
			}
		}
	}
	
	smol_swap_buffers(t_smol);
}