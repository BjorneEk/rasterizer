#include "include/triangle.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <util/util.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////

float z_avg(triangle_t tri) {
  //return (tri.p1.vec.z + tri.p2.vec.z + tri.p3.vec.z) / 3;

  return (tri.p1.vec.z > tri.p2.vec.z) ?
      ((tri.p1.vec.z > tri.p3.vec.z) ? tri.p1.vec.z : tri.p3.vec.z) :
      ((tri.p3.vec.z > tri.p2.vec.z) ? tri.p3.vec.z : tri.p2.vec.z);
}

bool tri_contains(triangle_t tri, vnpair_t p)
{
  return equals_v3d(tri.p1.vec, p.vec) || equals_v3d(tri.p2.vec, p.vec) || equals_v3d(tri.p3.vec, p.vec);
}

bool equals_tri(triangle_t t1, triangle_t t2)
{
  return tri_contains(t1, t2.p1) && tri_contains(t1, t2.p2) && tri_contains(t1, t2.p3);
}

float tri_compare_z(triangle_t t1, triangle_t t2)
{
  return z_avg(t1) - z_avg(t2);
}

triangle_t nulltri()
{
  return (triangle_t)
  {
    .p[0] = (vnpair_t){(vec3d_t){0.0,0.0,0.0}, (vec3d_t){0.0,0.0,0.0}},
    .p[1] = (vnpair_t){(vec3d_t){0.0,0.0,0.0}, (vec3d_t){0.0,0.0,0.0}},
    .p[2] = (vnpair_t){(vec3d_t){0.0,0.0,0.0}, (vec3d_t){0.0,0.0,0.0}}
  };
}

void print_trig(triangle_t tri) {
  printf("[ (%f, %f, %f) | (%f, %f, %f) | (%f, %f, %f)] -- ",
  tri.p[0].vec.x, tri.p[0].vec.y, tri.p[0].vec.z,
  tri.p[1].vec.x, tri.p[1].vec.y, tri.p[1].vec.z,
  tri.p[2].vec.x, tri.p[2].vec.y, tri.p[2].vec.z);
  /*
  printf("[ (%f, %f, %f) | (%f, %f, %f) | (%f, %f, %f)]\n",
  tri.p[0].normal.x, tri.p[0].normal.y, tri.p[0].normal.z,
  tri.p[1].normal.x, tri.p[1].normal.y, tri.p[1].normal.z,
  tri.p[2].normal.x, tri.p[2].normal.y, tri.p[2].normal.z);*/
  printf("\n");
}

vec3d_t normal(triangle_t trig) {
  vec3d_t v1 = vec_sub(trig.p1.vec, trig.p2.vec);
  vec3d_t v2 = vec_sub(trig.p1.vec, trig.p3.vec);
  vec3d_t cp = cross_prod(v1, v2);
  normalize(&cp);
  return cp;
}
vec3d_t vec_normal(vec3d_t a, vec3d_t b, vec3d_t c) {
  vec3d_t v1 = vec_sub(a, b);
  vec3d_t v2 = vec_sub(a, c);
  vec3d_t cp = cross_prod(v1, v2);
  normalize(&cp);
  return cp;
}

triangle_t rotate_tri_y(triangle_t t, double ang)
{
  return (triangle_t)
  {
    (vnpair_t){rotate_y(t.p1.vec, ang), rotate_y(t.p1.normal, ang)},
    (vnpair_t){rotate_y(t.p2.vec, ang), rotate_y(t.p2.normal, ang)},
    (vnpair_t){rotate_y(t.p3.vec, ang), rotate_y(t.p3.normal, ang)}
  };
}
triangle_t rotate_tri_x(triangle_t t, double ang)
{
  return (triangle_t)
  {
    (vnpair_t){rotate_x(t.p1.vec, ang), rotate_x(t.p1.normal, ang)},
    (vnpair_t){rotate_x(t.p2.vec, ang), rotate_x(t.p2.normal, ang)},
    (vnpair_t){rotate_x(t.p3.vec, ang), rotate_x(t.p3.normal, ang)}
  };
}

triangle_t rotate_tri(triangle_t t, double x, double y)
{
  triangle_t res;
  res = rotate_tri_x(t, x);
  res = rotate_tri_y(res, y);
  return res;
}

triangle_t project_tri(triangle_t t, matrix4x4_t pm)
{
  return (triangle_t)
  {
    (vnpair_t){mat4x4_mult(t.p1.vec, pm), mat4x4_mult(t.p1.normal, pm)},
    (vnpair_t){mat4x4_mult(t.p2.vec, pm), mat4x4_mult(t.p2.normal, pm)},
    (vnpair_t){mat4x4_mult(t.p3.vec, pm), mat4x4_mult(t.p3.normal, pm)}
  };
}

triangle_t scale_tri(triangle_t t, double scale)
{
  return (triangle_t)
  {
    (vnpair_t){vec_scale(t.p1.vec, scale), vec_scale(t.p1.normal, scale)},
    (vnpair_t){vec_scale(t.p2.vec, scale), vec_scale(t.p2.normal, scale)},
    (vnpair_t){vec_scale(t.p3.vec, scale), vec_scale(t.p3.normal, scale)}
  };
}

void scale_to_window(triangle_t * tri, float height, float width)
{
  tri->p1.vec.x += 1.0; tri->p1.vec.y += 1.0;
  tri->p2.vec.x += 1.0; tri->p2.vec.y += 1.0;
  tri->p3.vec.x += 1.0; tri->p3.vec.y += 1.0;

  tri->p1.vec.x *= 0.5f*2000.0f; tri->p1.vec.y *= 0.5f*1200.0f;
  tri->p2.vec.x *= 0.5f*2000.0f; tri->p2.vec.y *= 0.5f*1200.0f;
  tri->p3.vec.x *= 0.5f*2000.0f; tri->p3.vec.y *= 0.5f*1200.0f;
}

triangle_t translate_tri(triangle_t t, vec3d_t rel)
{
  return (triangle_t)
  {
    (vnpair_t){vec_add(t.p1.vec, rel), vec_add(t.p1.normal, rel)},
    (vnpair_t){vec_add(t.p2.vec, rel), vec_add(t.p2.normal, rel)},
    (vnpair_t){vec_add(t.p3.vec, rel), vec_add(t.p3.normal, rel)}
  };
}

triangle_t * get_triangles(obj_t * obj)
{
  triangle_t * res;

  res = malloc(obj->nfaces * sizeof(triangle_t));

  for (size_t i = 0; i < obj->nfaces; i++) {
    triangle_t new_tri;
    for (size_t j = 0; j < 3; j++) {
      vnpair_t new;
      u32_t    idx;

       /* index of actual vertex in vertex array */
      idx        = obj->faces[i].vertecies[j];
      new.vec    = from_vert(obj->vertecies[idx]);
      /*if(obj->has_normals)
      {
        new.normal = from_vertn(obj->vertex_normals[idx]);
      }
      else
      {*/
        new.normal = (vec3d_t){0.0,0.0,0.0};

         /* find all triangles containint the vertex and add theri normals to new.normal */
        for (size_t k = 0; k < obj->nfaces; k++) {
          if (obj->faces[k].vertecies[0] == idx || obj->faces[k].vertecies[1] == idx || obj->faces[k].vertecies[2] == idx)
          {
            vec3d_t a = from_vert(obj->vertecies[obj->faces[k].vertecies[0]]);
            vec3d_t b = from_vert(obj->vertecies[obj->faces[k].vertecies[1]]);
            vec3d_t c = from_vert(obj->vertecies[obj->faces[k].vertecies[2]]);
            new.normal = vec_add(new.normal, vec_normal(a, b, c));
          }
      //	}
      }
      normalize(&new.normal);
      new_tri.p[j] = new;
    }
    res[i] = new_tri;
  }
  return res;
}
void sort_mesh(triangle_t * tris, int first, int last)
{
  int i, j;
  int pivot;
  triangle_t tmp;

  if(first < last) {
    pivot = first;
    i     = first;
    j     = last;

    while(i < j) {
      while(tri_compare_z(tris[i], tris[pivot]) <= 0.0f && i < last) i++;
      while(tri_compare_z(tris[j], tris[pivot]) > 0.0f) j--;
      if( i < j ) {
        tmp     = tris[i];
        tris[i] = tris[j];
        tris[j] = tmp;
      }
    }

    tmp         = tris[pivot];
    tris[pivot] = tris[j];
    tris[j]     = tmp;
    sort_mesh(tris, first, j - 1);
    sort_mesh(tris, j + 1, last);
  }
}
