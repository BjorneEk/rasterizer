#include "include/vector.h"
#include "include/matrix.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////


vec3d_t from_vert(vert_t v)
{
  return (vec3d_t){ v.x, v.y, v.z };
}
vec3d_t from_vertn(vertn_t v)
{
  return (vec3d_t){ v.x, v.y, v.z };
}
float length_3d(const vec3d_t vec)
{
  return sqrt( fabsf( (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) ) );
}

/// vector-vector arithmetic
vec3d_t vec_add(vec3d_t v1, vec3d_t v2)
{
  return (vec3d_t){ (v1.x + v2.x), (v1.y + v2.y), (v1.z + v2.z) };
}
vec3d_t vec_sub(vec3d_t v1, vec3d_t v2)
{
  return (vec3d_t){ (v1.x - v2.x), (v1.y - v2.y), (v1.z - v2.z) };
}
vec3d_t vec_mult(vec3d_t v1, vec3d_t v2)
{
  return (vec3d_t){ (v1.x * v2.x), (v1.y * v2.y), (v1.z * v2.z) };
}
vec3d_t vec_div(vec3d_t v1, vec3d_t v2)
{
  return (vec3d_t){ (v1.x / v2.x), (v1.y / v2.y), (v1.z / v2.z) };
}


/// vector-scalar arithmetic
vec3d_t vec_addsc(vec3d_t v1, float s)
{
  return (vec3d_t){ (v1.x + s), (v1.y + s), (v1.z + s) };
}
vec3d_t vec_subsc(vec3d_t v1, float s)
{
  return (vec3d_t){ (v1.x - s), (v1.y - s), (v1.z - s) };
}
vec3d_t vec_scale(vec3d_t v1, float s)
{
  return (vec3d_t){ (v1.x * s), (v1.y * s), (v1.z * s) };
}
vec3d_t vec_divsc(vec3d_t v1, float s)
{
  return (vec3d_t){ (v1.x / s), (v1.y / s), (v1.z / s) };
}

char * v3d_to_str(vec3d_t vec)
{
  char * res = malloc(33 * sizeof(char));
  sprintf(res, "{%f | %f | %f}", vec.x, vec.y, vec.z);
  return res;
}

bool equals_v3d(vec3d_t v1, vec3d_t v2)
{
  return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

vec3d_t cross_prod(vec3d_t v1, vec3d_t v2) {
  return (vec3d_t)
  {
    .x = v1.y * v2.z - v1.z * v2.y,
    .y = v1.z * v2.x - v1.x * v2.z,
    .z = v1.x * v2.y - v1.y * v2.x
  };
}

void normalize(vec3d_t * v)
{
  float l = length_3d(*v);
  v->x /= l;
  v->y /= l;
  v->z /= l;
}

vec3d_t normalize_from(float a, float b, float c)
{
  vec3d_t v = (vec3d_t){a, b, c};
  float l = length_3d(v);
  v.x /= l;
  v.y /= l;
  v.z /= l;
  return v;
}

vec3d_t vec_normal(vec3d_t u, vec3d_t v, vec3d_t w) {
  vec3d_t v1 = vec_sub(u, v);
  vec3d_t v2 = vec_sub(u, w);
  vec3d_t cp = cross_prod(v1, v2);
  normalize(&cp);
  return cp;
}

float dot_prod(vec3d_t v1, vec3d_t v2)
{
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}
