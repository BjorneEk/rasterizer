#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "vector.h"
#include "matrix.h"
#include "triangle.h"
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////

typedef struct tri_idx {
  int p1, p2, p3;
} tri_idx_t;

typedef struct tri_arr {
  tri_idx_t * tris;
  int length;
} tri_arr_t;

typedef struct object {

  triangle_t * mesh;
  int tri_count;

  vec3d_t pos, rotation;

  double scale;

} object_t;

typedef struct _object {

  vertex_t * vertecies;
  int vert_count;


  tri_idx_t * tris;
  int tri_count;


  /**
   * list of triangles containing a certain vertex
   **/
  tri_arr_t * vec_to_tris;

  vec3d_t pos, rotation;

  double scale;

} _object_t;

object_t new_object(triangle_t * mesh, int tri_count);

void update_normals(_object_t * obj);
int convert_object(_object_t * res, obj_t * obj);
_object_t shallow_copy_object(const _object_t obj);

triangle_t get_tri(const _object_t obj, int i);

vec3d_t obj_normal_tri(const _object_t obj, int i);
void shallow_free_obj(_object_t obj);
void print_object(_object_t obj);

double object_height(object_t obj);
double object_width(object_t obj);
double object_depth(object_t obj);
void color_by_height(object_t * obj, double offset);
object_t wave(unsigned int width, unsigned int depth, float scale, float amp);
object_t create_plane(unsigned int width, unsigned int depth);
_object_t _create_plane(unsigned int width, unsigned int depth);
void randomize(_object_t * obj);

#endif /* _OBJECT_H_ */
