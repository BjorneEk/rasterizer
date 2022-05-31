
#include "include/object.h"
#include "include/color.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////


void update_normals(_object_t * obj) {
  for (size_t i = 0; i < obj->vert_count; i++) {
    obj->vertecies[i].normal = (vec3d_t){0,0,0};
    for(size_t j = 0; j < obj->vec_to_tris[i].length; j++) {
      obj->vertecies[i].normal = vec_add(obj->vertecies[i].normal, vec_normal(
        obj->vertecies[obj->vec_to_tris[i].tris[j].p1].vec,
        obj->vertecies[obj->vec_to_tris[i].tris[j].p2].vec,
        obj->vertecies[obj->vec_to_tris[i].tris[j].p3].vec
      ));
    }
    normalize(&obj->vertecies[i].normal);
  }
}

int convert_object(_object_t * res, obj_t * obj)
{
  res->vert_count = obj->nverts;
  res->tri_count  = obj->nfaces;
  res->pos      = (vec3d_t){0.0,0.0,0.0};
  res->rotation = (vec3d_t){0.0,0.0,0.0};
  res->scale = 1.0;
  /**
   *  copy vertecies and triangle indicies to res
   **/
  res->vertecies   = malloc(obj->nverts * sizeof(vertex_t));
  res->tris        = malloc(obj->nfaces * sizeof(tri_idx_t));
  res->vec_to_tris = malloc(obj->nverts * sizeof(tri_arr_t));

  for(size_t i = 0; i < obj->nverts; i++) {
    vec3d_t n = (vec3d_t){0,0,0};
    if (obj->has_normals) n = (vec3d_t){obj->vertex_normals[i].x, obj->vertex_normals[i].y, obj->vertex_normals[i].z};
    res->vertecies[i] = (vertex_t){
      (vec3d_t){obj->vertecies[i].x,obj->vertecies[i].y,obj->vertecies[i].z},
      n,
      COLOR_WHITE
    };
  }
  for(size_t i = 0; i < obj->nfaces; i++) {
    if (obj->faces[i].nverts != 3) return -1;
    res->tris[i].p1 = obj->faces[i].vertecies[0];
    res->tris[i].p2 = obj->faces[i].vertecies[1];
    res->tris[i].p3 = obj->faces[i].vertecies[2];
  }

  /**
   *  create list of all triangles containing a certain vertex
   **/
  for (size_t i = 0; i < res->vert_count; i++) {
    res->vec_to_tris[i].tris = malloc(res->tri_count * sizeof(tri_idx_t));
    res->vec_to_tris[i].length = 0;

    for (size_t j = 0; j < res->tri_count; j++)
      if ((res->tris[j].p1 == i) || (res->tris[j].p2 == i) || (res->tris[j].p3 == i)) {
        res->vec_to_tris[i].tris[res->vec_to_tris[i].length] = res->tris[j];
        res->vec_to_tris[i].length++;
      }
  }
  update_normals(res);
  return 0;
}

void print_object(_object_t obj)
{
  printf("┌───┤Object info├───*\n│\n");
  for (size_t i = 0; i < obj.tri_count; i++)
  {
    printf("├──( ");
    vertex_t tmp = obj.vertecies[obj.tris[i].p1];
    printf("< %f %f %f > ", tmp.vec.x, tmp.vec.y, tmp.vec.z);
    tmp = obj.vertecies[obj.tris[i].p2];
    printf("< %f %f %f > ", tmp.vec.x, tmp.vec.y, tmp.vec.z);
    tmp = obj.vertecies[obj.tris[i].p3];
    printf("< %f %f %f > ", tmp.vec.x, tmp.vec.y, tmp.vec.z);
    printf(")\n");
  }
  printf("└───────────────────\n");
  printf("┌───┤normals├───*\n│\n");
  for(size_t i = 0; i < obj.vert_count; i++) {
    vertex_t tmp = obj.vertecies[obj.tris[i].p1];
    printf("├──( ");
    printf("< %f %f %f > ", tmp.normal.x, tmp.normal.y, tmp.normal.z);
    printf(")\n");
  }
  printf("└───────────────────\n");
}

_object_t shallow_copy_object(const _object_t obj) {
  _object_t res;
  res.pos         = obj.pos;
  res.rotation    = obj.rotation;
  res.scale       = obj.scale;
  res.tri_count   = obj.tri_count;
  res.vert_count  = obj.vert_count;
  res.vertecies   = malloc(obj.vert_count * sizeof(vertex_t));
  res.tris        = malloc(obj.tri_count * sizeof(tri_idx_t));
  res.vec_to_tris = malloc(obj.vert_count * (sizeof(tri_idx_t *) + sizeof(int)));
  memcpy(res.vertecies, obj.vertecies, obj.vert_count * sizeof(vertex_t));
  memcpy(res.tris, obj.tris, obj.tri_count * sizeof(tri_idx_t));
  memcpy(res.vec_to_tris, obj.vec_to_tris, obj.vert_count * (sizeof(tri_idx_t *) + sizeof(int)));
  return res;
}

void shallow_free_obj(_object_t obj) {
  free(obj.tris);
  free(obj.vertecies);
  free(obj.vec_to_tris[1].tris);
}
triangle_t get_tri(const _object_t obj, int i) {
  return (triangle_t) {
    obj.vertecies[obj.tris[i].p1],
    obj.vertecies[obj.tris[i].p1],
    obj.vertecies[obj.tris[i].p1]
  };
}

vec3d_t obj_normal_tri(const _object_t obj, int i) {
  return vec_normal(obj.vertecies[obj.tris[i].p1].vec, obj.vertecies[obj.tris[i].p1].vec, obj.vertecies[obj.tris[i].p1].vec);
}




object_t new_object(triangle_t * mesh, int tri_count) {
  return (object_t) {
    mesh,
    tri_count,
    (vec3d_t){0.0,0.0,0.0},(vec3d_t){0.0,0.0,0.0}, // orgin position and rotation
    1.0
  };
}

double object_height(object_t obj) {
  double max;
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = min = obj.mesh[i].p1.vec.y;
    if (max < obj.mesh[i].p1.vec.y)      max = obj.mesh[i].p1.vec.y;
    else if (min > obj.mesh[i].p1.vec.y) min = obj.mesh[i].p1.vec.y;
    if (max < obj.mesh[i].p2.vec.y)      max = obj.mesh[i].p2.vec.y;
    else if (min > obj.mesh[i].p2.vec.y) min = obj.mesh[i].p2.vec.y;
    if (max < obj.mesh[i].p3.vec.y)      max = obj.mesh[i].p3.vec.y;
    else if (min > obj.mesh[i].p3.vec.y) min = obj.mesh[i].p3.vec.y;
  }
  return max-min;
}
double object_width(object_t obj) {
  double max;
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = min = obj.mesh[i].p1.vec.x;
    if (max < obj.mesh[i].p1.vec.x)      max = obj.mesh[i].p1.vec.x;
    else if (min > obj.mesh[i].p1.vec.x) min = obj.mesh[i].p1.vec.x;
    if (max < obj.mesh[i].p2.vec.x)      max = obj.mesh[i].p2.vec.x;
    else if (min > obj.mesh[i].p2.vec.x) min = obj.mesh[i].p2.vec.x;
    if (max < obj.mesh[i].p3.vec.x)      max = obj.mesh[i].p3.vec.x;
    else if (min > obj.mesh[i].p3.vec.x) min = obj.mesh[i].p3.vec.x;
  }
  return max-min;
}
double object_depth(object_t obj) {
  double max;
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = min = obj.mesh[i].p1.vec.z;
    if (max < obj.mesh[i].p1.vec.z)      max = obj.mesh[i].p1.vec.z;
    else if (min > obj.mesh[i].p1.vec.z) min = obj.mesh[i].p1.vec.z;
    if (max < obj.mesh[i].p2.vec.z)      max = obj.mesh[i].p2.vec.z;
    else if (min > obj.mesh[i].p2.vec.z) min = obj.mesh[i].p2.vec.z;
    if (max < obj.mesh[i].p3.vec.z)      max = obj.mesh[i].p3.vec.z;
    else if (min > obj.mesh[i].p3.vec.z) min = obj.mesh[i].p3.vec.z;
  }
  return max-min;
}
double object_max_height(object_t obj) {
  double max;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = obj.mesh[i].p1.vec.y;
    if (max < obj.mesh[i].p1.vec.y)      max = obj.mesh[i].p1.vec.y;
    if (max < obj.mesh[i].p2.vec.y)      max = obj.mesh[i].p2.vec.y;
    if (max < obj.mesh[i].p3.vec.y)      max = obj.mesh[i].p3.vec.y;
  }
  return max;
}
double object_min_height(object_t obj) {
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) min = obj.mesh[i].p1.vec.y;
    if (min > obj.mesh[i].p1.vec.y) min = obj.mesh[i].p1.vec.y;
    if (min > obj.mesh[i].p2.vec.y) min = obj.mesh[i].p2.vec.y;
    if (min > obj.mesh[i].p3.vec.y) min = obj.mesh[i].p3.vec.y;
  }
  return min;
}

double object_max_depth(object_t obj) {
  double max;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = obj.mesh[i].p1.vec.z;
    if (max < obj.mesh[i].p1.vec.z)      max = obj.mesh[i].p1.vec.z;
    if (max < obj.mesh[i].p2.vec.z)      max = obj.mesh[i].p2.vec.z;
    if (max < obj.mesh[i].p3.vec.z)      max = obj.mesh[i].p3.vec.z;
  }
  return max;
}
double object_min_depth(object_t obj) {
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) min = obj.mesh[i].p1.vec.z;
    if (min > obj.mesh[i].p1.vec.z) min = obj.mesh[i].p1.vec.z;
    if (min > obj.mesh[i].p2.vec.z) min = obj.mesh[i].p2.vec.z;
    if (min > obj.mesh[i].p3.vec.z) min = obj.mesh[i].p3.vec.z;
  }
  return min;
}

double object_max_width(object_t obj) {
  double max;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) max = obj.mesh[i].p1.vec.x;
    if (max < obj.mesh[i].p1.vec.x)      max = obj.mesh[i].p1.vec.x;
    if (max < obj.mesh[i].p2.vec.x)      max = obj.mesh[i].p2.vec.x;
    if (max < obj.mesh[i].p3.vec.x)      max = obj.mesh[i].p3.vec.x;
  }
  return max;
}
double object_min_width(object_t obj) {
  double min;
  for(int i = 0; i < obj.tri_count; i++) {
    if (i == 0) min = obj.mesh[i].p1.vec.x;
    if (min > obj.mesh[i].p1.vec.x) min = obj.mesh[i].p1.vec.x;
    if (min > obj.mesh[i].p2.vec.x) min = obj.mesh[i].p2.vec.x;
    if (min > obj.mesh[i].p3.vec.x) min = obj.mesh[i].p3.vec.x;
  }
  return min;
}

void color_by_height(object_t * obj, double offset) {
  double miny = object_min_height(*obj);
  double maxy = object_max_height(*obj);
  double minx = object_min_width(*obj);
  double maxx = object_max_width(*obj);
  for(int i = 0; i < obj->tri_count; i++) {
    obj->mesh[i].p1.color = from_angle((((50.0) / (maxy - miny)) * (obj->mesh[i].p1.vec.y - miny)) +
      offset + 1.2*sin((((40.0) / (maxx - minx)) * (obj->mesh[i].p1.vec.x - minx))));
    obj->mesh[i].p2.color = from_angle((((50.0) / (maxy - miny)) * (obj->mesh[i].p2.vec.y - miny)) +
      offset + 1.2*sin((((40.0) / (maxx - minx)) * (obj->mesh[i].p2.vec.x - minx))));
    obj->mesh[i].p3.color = from_angle((((50.0) / (maxy - miny)) * (obj->mesh[i].p3.vec.y - miny)) +
      offset + 1.2*sin((((40.0) / (maxx - minx)) * (obj->mesh[i].p3.vec.x - minx))));
  }
}




object_t wave(unsigned int width, unsigned int depth, float scale, float amp) {
  triangle_t * plane;
  int cnt = 0;
  vec3d_t n = (vec3d_t){0.0,1.0,0.0};
  plane = malloc(2 * width * depth * sizeof(triangle_t));
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < depth; j++) {
      plane[cnt] = (triangle_t) {
        (vertex_t){(vec3d_t){i,  amp*sin(i*scale),j  },     (vec3d_t){sin(i*scale),cos(i*scale),0.0}, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,amp*sin((i+1)*scale),j+1}, (vec3d_t){sin((i+1)*scale),cos((i+1)*scale),0.0}, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,amp*sin((i+1)*scale),j  }, (vec3d_t){sin((i+1)*scale),cos((i+1)*scale),0.0}, COLOR_WHITE}
      };
      plane[cnt+1] = (triangle_t) {
        (vertex_t){(vec3d_t){i,  amp*sin(i*scale),j  },     (vec3d_t){sin(i*scale),cos(i*scale),0.0}, COLOR_WHITE},
        (vertex_t){(vec3d_t){i  ,amp*sin(i*scale),j+1},     (vec3d_t){sin(i*scale),cos(i*scale),0.0}, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,amp*sin((i+1)*scale),j+1}, (vec3d_t){sin((i+1)*scale),cos((i+1)*scale),0.0}, COLOR_WHITE}
      };
      cnt += 2;
    }
  }
  return new_object(plane, cnt);
}

object_t create_plane(unsigned int width, unsigned int depth) {
  triangle_t * plane;
  int cnt = 0;
  vec3d_t n = (vec3d_t){0.0,1.0,0.0};
  plane = malloc(2 * width * depth * sizeof(triangle_t));
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < depth; j++) {
      plane[cnt] = (triangle_t) {
        (vertex_t){(vec3d_t){i,  0,j  }, n, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,0,j+1}, n, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,0,j  }, n, COLOR_WHITE}
      };
      plane[cnt+1] = (triangle_t) {
        (vertex_t){(vec3d_t){i,  0,j  }, n, COLOR_WHITE},
        (vertex_t){(vec3d_t){i  ,0,j+1}, n, COLOR_WHITE},
        (vertex_t){(vec3d_t){i+1,0,j+1}, n, COLOR_WHITE}
      };
      cnt += 2;
    }
  }
  return new_object(plane, cnt);
}

void randomize(_object_t * obj) {
  for (int i = 0; i < obj->vert_count; i++) {
    obj->vertecies[i].vec = vec_add(obj->vertecies[i].vec, (vec3d_t){0.0,2.0/rand(),0.0});
  }
  update_normals(obj);
}

_object_t _create_plane(unsigned int width, unsigned int depth) {
  _object_t res;
  res.vert_count = width*depth;
  res.pos      = (vec3d_t){0.0,0.0,0.0};
  res.rotation = (vec3d_t){0.0,0.0,0.0};
  res.scale = 1.0;

  /**
   *  copy vertecies and triangle indicies to res
   **/
  res.vertecies   = malloc(res.vert_count * sizeof(vertex_t));
  res.tris        = malloc(2*res.vert_count * sizeof(tri_idx_t));
  res.vec_to_tris = malloc(res.vert_count * sizeof(tri_arr_t));

  int cnt = 0;
  vec3d_t n = (vec3d_t){0.0,1.0,0.0};

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < depth; j++) {
      res.vertecies[i+(j*width)] = (vertex_t){(vec3d_t){i,sin(i*0.5)+ (0.01*(rand()%30)),j}, n, COLOR_WHITE};
      if ((i < (width-1)) && (j < (depth-1))) {
        res.tris[cnt].p1 = i+(j*width);
        res.tris[cnt].p2 = i+1+((j+1)*width);
        res.tris[cnt].p3 = (i+1)+(j*width);

        res.tris[cnt+1].p1 = (i  )+((j  )*width);
        res.tris[cnt+1].p2 = (i  )+((j+1)*width);
        res.tris[cnt+1].p3 = (i+1)+((j+1)*width);
        cnt += 2;
      }
    }
  }
  res.tri_count = cnt;
  for (size_t i = 0; i < res.vert_count; i++) {
    res.vec_to_tris[i].tris = malloc(10 * sizeof(tri_idx_t));
    res.vec_to_tris[i].length = 0;

    for (size_t j = 0; j < res.tri_count; j++)
      if ((res.tris[j].p1 == i) || (res.tris[j].p2 == i) || (res.tris[j].p3 == i)) {
        res.vec_to_tris[i].tris[res.vec_to_tris[i].length] = res.tris[j];
        res.vec_to_tris[i].length++;
      }
  }
  update_normals(&res);
  return res;
}


vec3d_t raycast_plane(vec3d_t normal, vec3d_t p_plane, vec3d_t p_line, vec3d_t lv) {
  float d = dot_prod(normal, p_plane);

  double t;
  for (t = 0; dot_prod(normal, vec_add(p_line, vec_scale(lv, t)))-d <= 0; t += 0.0001);
  return vec_add(p_line, vec_scale(lv, t));
}

vec3d_t project_point(object_t plane, vec3d_t p, vec3d_t v) {
  vec3d_t normal = cross_prod(vec_sub(plane.mesh[0].p2.vec, plane.mesh[0].p1.vec),
                                vec_sub(plane.mesh[0].p3.vec, plane.mesh[0].p1.vec));
  return raycast_plane(normal, plane.pos, p, v);
}
void cast_shaddow(object_t obj, object_t target, vec3d_t source) {

}
