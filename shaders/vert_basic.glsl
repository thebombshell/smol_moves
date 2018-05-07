
layout(location = 0) in vec3 attribute_position;

#ifdef USE_WORLD_MATRIX
#define MUL_BY_WORLD transpose(uniform_world) *

uniform mat4 uniform_world;

#else
#define MUL_BY_WORLD
#endif

#ifdef USE_VIEW_MATRIX
#define MUL_BY_VIEW uniform_view *

uniform mat4 uniform_view;

#else
#define MUL_BY_VIEW
#endif

#ifdef USE_PROJ_MATRIX
#define MUL_BY_PROJ transpose(uniform_proj) *

uniform mat4 uniform_proj;

#else
#define MUL_BY_PROJ
#endif

void main(void) {

	gl_Position = MUL_BY_PROJ MUL_BY_VIEW MUL_BY_WORLD vec4(attribute_position, 1.0); 
}