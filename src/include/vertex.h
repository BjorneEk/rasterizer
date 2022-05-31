#ifndef _VERTEX_H_
#define _VERTEX_H_


////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include "wobjl.h"
#include "color.h"
#include "vector.h"
#include "matrix.h"

typedef struct vertex
{
  vec3d_t vec;
  vec3d_t normal;
  color_t color;
  float light;
} vertex_t;

vertex_t translate_vert(vertex_t vert, vec3d_t pos);

vertex_t rotate_vert(vertex_t vert, vec3d_t rot);

vertex_t scale(vertex_t vert, double size);

void sort_vertecies(vertex_t * verts, int first, int last);

#endif /* _VERTEX_H_ */
