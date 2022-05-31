#ifndef _VECTOR_H_
#define _VECTOR_H_


////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include "wobjl.h"
#include "color.h"

typedef struct vec3d
{
  float x;
  float y;
  float z;
} vec3d_t;





inline float length_3d(const vec3d_t vec);


vec3d_t from_vert(vert_t v);
vec3d_t from_vertn(vertn_t v);
///
/// standard vector - vector arithmetic
///
vec3d_t vec_add(vec3d_t v1, vec3d_t v2);
vec3d_t vec_sub(vec3d_t v1, vec3d_t v2);
vec3d_t vec_mult(vec3d_t v1, vec3d_t v2);
vec3d_t vec_div(vec3d_t v1, vec3d_t v2);

///
/// standard vector - scalar arithmetic
///
vec3d_t vec_addsc(vec3d_t v1, float scalar);
vec3d_t vec_subsc(vec3d_t v1, float scalar);
vec3d_t vec_scale(vec3d_t v1, float scalar);
vec3d_t vec_divsc(vec3d_t v1, float scalar);

char * v3d_to_str(vec3d_t vec);

bool equals_v3d(vec3d_t v1, vec3d_t v2);

vec3d_t cross_prod(vec3d_t v1, vec3d_t v2);
float dot_prod(vec3d_t v1, vec3d_t v2);

void normalize(vec3d_t * v);

vec3d_t normalize_from(float a, float b, float c);

vec3d_t vec_normal(vec3d_t u, vec3d_t v, vec3d_t w);

float cmp_vec(vec3d_t v1, vec3d_t v2);


#endif /* _VECTOR_H_ */
