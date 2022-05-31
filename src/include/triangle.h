#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include "vector.h"
#include "matrix.h"
#include "wobjl.h"
#include "vertex.h"
#include <stdio.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////

typedef struct triangle
{
  union
  {
    struct
    {
      vertex_t p1;
      vertex_t p2;
      vertex_t p3;
    };
    vertex_t p[3];
  };
} triangle_t;

float z_avg(triangle_t tri);

bool tri_contains(triangle_t tri, vertex_t p);
bool equals_tri(triangle_t t1, triangle_t t2);

void print_trig(triangle_t tri);

vec3d_t normal(triangle_t trig);

triangle_t * get_triangles(obj_t * obj);

triangle_t rotate_tri(triangle_t t, vec3d_t angles);

triangle_t scale_tri(triangle_t t, double scale);

triangle_t translate_tri(triangle_t t, vec3d_t rel);

triangle_t project_tri(triangle_t t, matrix4x4_t pm);

void sort_mesh(triangle_t * tris, int first, int last);

void scale_to_window(triangle_t * tri, float height, float width);

#endif /* _TRIANGLE_H_ */
