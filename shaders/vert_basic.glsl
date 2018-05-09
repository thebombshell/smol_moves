
layout(location = 0) in vec3 attribute_position;

uniform mat4 uniform_world;
uniform mat4 uniform_view;
uniform mat4 uniform_proj;

void main(void) {

	gl_Position = uniform_proj * uniform_view * uniform_world * vec4(attribute_position, 1.0); 
}