
layout(location = 0) out vec4 out_color;

void main(void) {
	
	out_color = vec4(vec3(1. - gl_FragCoord.z) + .5, 1.);
}