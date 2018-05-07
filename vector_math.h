
#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#define _USE_MATH_DEFINES

#include "window_renderer.h"
#include <math.h>

#define DECLARE_VECTOR_METHOD_GROUP( SIZE ) \
	float* vec ## SIZE ## _copy(float* t_target, float* t_b); \
	float* vec ## SIZE ## _add(float* t_target, float* t_a, float* t_b); \
	float* vec ## SIZE ## _sub(float* t_target, float* t_a, float* t_b); \
	float* vec ## SIZE ## _mul(float* t_target, float* t_a, float* t_b); \
	float* vec ## SIZE ## _div(float* t_target, float* t_a, float* t_b); \
	float* vec ## SIZE ## _adds(float* t_target, float* t_a, float t_b); \
	float* vec ## SIZE ## _subs(float* t_target, float* t_a, float t_b); \
	float* vec ## SIZE ## _muls(float* t_target, float* t_a, float t_b); \
	float* vec ## SIZE ## _divs(float* t_target, float* t_a, float t_b); \
	float vec ## SIZE ## _dot(float* t_a, float* t_b); \
	float vec ## SIZE ## _lensq(float* t_a); \
	float vec ## SIZE ## _length(float* t_a); \
	float* vec ## SIZE ## _normalize(float* t_target, float* t_a);

DECLARE_VECTOR_METHOD_GROUP( 2 )
DECLARE_VECTOR_METHOD_GROUP( 3 )
DECLARE_VECTOR_METHOD_GROUP( 4 )

float* vec3_cross(float* t_target, float* t_a, float* t_b);
float* vec3_rotate(float* t_target, float* t_a, float* t_b);

float* quat_identity(float* t_target);
float* quat_axis_angle(float* t_target, float* t_axis, float t_angle);
float* quat_conjugate(float* t_target, float* t_a);
float* quat_mul(float* t_target, float* t_a, float* t_b);
#define quat_copy( ... ) vec4_copy( __VA_ARGS__ )
#define quat_lensq( ... ) vec4_lensq( __VA_ARGS__ )
#define quat_length( ... ) vec4_length( __VA_ARGS__ )
#define quat_normalize( ... ) vec4_normalize( __VA_ARGS__ )

#define DECLARE_MATRIX_METHOD_GROUP( SIZE ) \
	float* mat ## SIZE ## _identity(float* t_target); \
	float* mat ## SIZE ## _copy(float* t_target, float* t_b); \
	float* mat ## SIZE ## _minor(float* t_target, float* t_b, unsigned int t_ignore_c, unsigned int t_ignore_r); \
	float* mat ## SIZE ## _mul(float* t_target, float* t_a, float* t_b); \
	float* mat ## SIZE ## _adds(float* t_target, float* t_a, float t_b); \
	float* mat ## SIZE ## _subs(float* t_target, float* t_a, float t_b); \
	float* mat ## SIZE ## _muls(float* t_target, float* t_a, float t_b); \
	float* mat ## SIZE ## _divs(float* t_target, float* t_a, float t_b); \
	float mat ## SIZE ## _determinant(float* t_a); \
	float* mat ## SIZE ## _minors(float* t_target, float* t_a); \
	float* mat ## SIZE ## _cofactors(float* t_target, float* t_a); \
	float* mat ## SIZE ## _transpose(float* t_target, float* t_a); \
	float* mat ## SIZE ## _adjugate(float* t_target, float* t_a); \
	float* mat ## SIZE ## _invert(float* t_target, float* t_a); \

DECLARE_MATRIX_METHOD_GROUP( 2 )
DECLARE_MATRIX_METHOD_GROUP( 3 )
DECLARE_MATRIX_METHOD_GROUP( 4 )

float* mat4_translation(float* t_target, float* t_translation);
float* mat4_rotation(float* t_target, float* t_rotation);
float* mat4_scaling(float* t_target, float* t_scale);
float* mat4_transformation(float* t_target, float* t_translation, float* t_rotation, float* t_scale);
float* mat4_lookat(float* t_target, float* t_eye, float* t_look, float* t_up);
float* mat4_frustum(float* t_target, float t_l, float t_r, float t_t, float t_b, float t_n, float t_f);
float* mat4_orthographic(float* t_target, float t_ar, float t_width, float t_n, float t_f);
float* mat4_perspective(float* t_target, float t_ar, float t_fov, float t_n, float t_f);

#endif