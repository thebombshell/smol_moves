
/**
 *
 *
 *
 */
#ifndef SHADER_H
#define SHADER_H

const char vs_cont_header[] = "#version 460\n";
const char vs_cont_vertex_pos_col_norm_uv[] =
	"layout(location = 0) in vec3 attribute_position;\n"
	"layout(location = 1) in vec3 attribute_normal;\n"
	"layout(location = 2) in vec2 attribute_uv;\n"
	"layout(location = 3) in vec4 attribute_colour;\n"
	"\n"
	"out vec3 vertex_position;\n"
	"out vec3 vertex_normal;\n"
	"out vec2 vertex_uv;\n"
	"out vec4 vertex_colour;\n"
	"\n";
const char vs_cont_uniform_wvp[] =
	"uniform mat4 uniform_world;\n"
	"uniform mat4 uniform_view;\n"
	"uniform mat4 uniform_proj;\n"
	"\n";
const char vs_cont_body_pos_col_norm_uv_wvp[] =
	"void main(void) {\n"
	"	gl_FragPosition = uniform_world * uniform_view * uniform_proj * vec4(attribute_position, 1.0);\n"
	"	vertex_position = uniform_world * attribute_position;\n"
	"	vertex_normal = mat3(uniform_world) * attribute_normal;\n"
	"	vertex_uv = attribute_uv;\n"
	"	vertex_colour = attribute_colour;\n"
	"}\n"
	"\n";

#endif