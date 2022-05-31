#include "include/vertex.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////




vertex_t translate_vert(vertex_t vert, vec3d_t pos) {
  return (vertex_t) {
    vec_add(vert.vec, pos),
    vert.normal,
    vert.color,
    vert.light
  };
}

vertex_t rotate_vert(vertex_t vert, vec3d_t rot) {
  return (vertex_t) {
    rotate_vec(vert.vec, rot),
    rotate_vec(vert.normal, rot),
    vert.color,
    vert.light
  };
}

vertex_t scale(vertex_t vert, double size) {
  return (vertex_t) {
    vec_scale(vert.vec, size),
    vert.normal,
    vert.color,
    vert.light
  };
}
void sort_vertecies(vertex_t * verts, int first, int last)
{
  int i, j;
  int pivot;
  vertex_t tmp;

  if(first < last) {
    pivot = first;
    i     = first;
    j     = last;

    while(i < j) {
      while((verts[i].vec.z - verts[pivot].vec.z) <= 0.0f && i < last) i++;
      while((verts[j].vec.z - verts[pivot].vec.z) > 0.0f) j--;
      if( i < j ) {
        tmp      = verts[i];
        verts[i] = verts[j];
        verts[j] = tmp;
      }
    }

    tmp          = verts[pivot];
    verts[pivot] = verts[j];
    verts[j]     = tmp;
    sort_vertecies(verts, first, j - 1);
    sort_vertecies(verts, j + 1, last);
  }
}
