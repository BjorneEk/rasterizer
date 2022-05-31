
////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////


#include <sdl/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include "include/raster.h"
#include "include/wobjl.h"
#include "include/triangle.h"
#include "include/object.h"
#include "include/matrix.h"

#define WIDTH  2000
#define HEIGHT 1200


typedef struct settings {
  bool smooth, wframe, fill;
  vec3d_t v_cam, v_light;
  matrix4x4_t proj_mat;
} settings_t;

static void log_warning(const char * msg)
{
  printf("[\033[33;1;4mWarning\033[0m]: %s\n", msg);
}

static void log_error(const char * msg)
{
  fprintf(stderr, "[\033[31;1;4mError\033[0m]: %s | %s\n",msg, strerror( errno ));
}

void init_window(SDL_Window ** window, SDL_Renderer ** renderer)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, window, renderer);

  if(*window == NULL || *renderer == NULL) {
    log_error("Failed to initialize SDL2 window");
    return;
  }

  SDL_SetWindowTitle(*window, "window");
  SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0);
  SDL_RenderClear(*renderer);
  SDL_RenderPresent(*renderer);
}

void print_mat(matrix4x4_t m)
{
  printf(" %f %f %f %f\n", m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0]);
  printf(" %f %f %f %f\n", m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1]);
  printf(" %f %f %f %f\n", m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2]);
  printf(" %f %f %f %f\n", m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
}
void print_mat3(matrix3x3_t m)
{
  printf(" %f %f %f \n", m.matrix[0][0], m.matrix[1][0], m.matrix[2][0]);
  printf(" %f %f %f \n", m.matrix[0][1], m.matrix[1][1], m.matrix[2][1]);
  printf(" %f %f %f \n", m.matrix[0][2], m.matrix[1][2], m.matrix[2][2]);
}


triangle_t proj_sharp_triangle(const triangle_t tri, matrix4x4_t p_mat, vec3d_t light){
  triangle_t res = project_tri(tri, p_mat);
  scale_to_window(&res, WIDTH, HEIGHT);
  float lf = dot_prod(normal(tri), light);
  if (lf > 1.0) lf = 1.0;
  res.p1.light = lf;
  res.p2.light = lf;
  res.p3.light = lf;
  return res;
}

vertex_t proj_smooth_vert(const vertex_t v, matrix4x4_t p_mat, vec3d_t light){
  vertex_t res = (vertex_t){mat4x4_mult(v.vec, p_mat), v.normal, v.color};
  res.vec.x += 1.0; res.vec.y += 1.0;
  res.vec.x *= 0.5f*2000.0f; res.vec.y *= 0.5f*1200.0f;
  res.light = dot_prod(res.normal, light);
  if (res.light > 1.0) res.light = 1.0;
  return res;
}

triangle_t proj_smooth_triangle(const triangle_t tri, matrix4x4_t p_mat, vec3d_t light){
  triangle_t res = project_tri(tri, p_mat);
  scale_to_window(&res, WIDTH, HEIGHT);
  float la = dot_prod(res.p1.normal, light);
  float lb = dot_prod(res.p2.normal, light);
  float lc = dot_prod(res.p3.normal, light);
  if (la > 1.0) la = 1.0; if (lb > 1.0) lb = 1.0; if (lc > 1.0) lc = 1.0;
  res.p1.light = la;
  res.p2.light = lb;
  res.p3.light = lc;
  return res;
}

void render_mesh(SDL_Renderer * renderer, object_t obj, object_t plane, settings_t settings) {

  triangle_t mesh[obj.tri_count];
  triangle_t plane_mesh[plane.tri_count];
  triangle_t projected_mesh[obj.tri_count + plane.tri_count];
  int tri_count, i;

  for (i = 0; i < obj.tri_count; i++)
    mesh[i] = translate_tri(scale_tri(rotate_tri(obj.mesh[i], obj.rotation), obj.scale), obj.pos);
  for (i = 0; i < plane.tri_count; i++)
    plane_mesh[i] = translate_tri(scale_tri(rotate_tri(plane.mesh[i], plane.rotation), plane.scale), plane.pos);

  tri_count = 0;

  for (i = 0; i < obj.tri_count; i++) {
    if (dot_prod(normal(mesh[i]), vec_sub(mesh[i].p1.vec, settings.v_cam)) < 0.0f) {
      projected_mesh[tri_count] = (settings.smooth) ?
        proj_smooth_triangle(mesh[i], settings.proj_mat, settings.v_light) :
        proj_sharp_triangle(mesh[i], settings.proj_mat, settings.v_light);
      tri_count++;
    }
  }
  for (i = 0; i < plane.tri_count; i++) {
    if (dot_prod(normal(plane_mesh[i]), vec_sub(plane_mesh[i].p1.vec, settings.v_cam)) < 0.0f) {
      projected_mesh[tri_count] = (settings.smooth) ?
        proj_smooth_triangle(plane_mesh[i], settings.proj_mat, settings.v_light) :
        proj_sharp_triangle(plane_mesh[i], settings.proj_mat, settings.v_light);
      tri_count++;
    }
  }
  sort_mesh(projected_mesh, 0, tri_count-1);
  for(int i = 0; i < tri_count; i++) {
    if (settings.wframe) {
      SDL_SetRenderDrawColor(renderer, 0, 244, 0, 1);
      draw_triangle(renderer, &projected_mesh[i].p1.vec, &projected_mesh[i].p2.vec, &projected_mesh[i].p3.vec);
    } else {
      raster_triangle(renderer, &projected_mesh[i].p1, &projected_mesh[i].p2, &projected_mesh[i].p3,
        projected_mesh[i].p1.light, projected_mesh[i].p2.light, projected_mesh[i].p3.light);
    }
  }
}

object_t obj_to_meshobject(_object_t obj) {
  triangle_t * mesh;

  mesh = malloc(obj.tri_count * sizeof(triangle_t));
  print_object(obj);
  for(int i = 0; i < obj.tri_count; i++) {
    mesh[i] = (triangle_t){
      obj.vertecies[obj.tris[i].p1],
      obj.vertecies[obj.tris[i].p2],
      obj.vertecies[obj.tris[i].p3]
    };
  }
  return (object_t) {
    mesh,
    obj.tri_count,
    obj.pos,
    obj.rotation,
    obj.scale
  };
}
/*  printf("┌───┤vecs├───*\n│\n");
  for(size_t i = 0; i < obj.vert_count; i++) {
    vertex_t tmp = verts[i];
    printf("├──( ");
    printf("< %f %f %f > ", tmp.vec.x, tmp.vec.y, tmp.vec.z);
    printf(")\n");
  }
  printf("└───────────────────\n");*/

void render_object(SDL_Renderer * renderer, _object_t obj, _object_t plane, settings_t settings) {
  triangle_t projected_mesh[obj.tri_count+plane.tri_count];
  int tri_count, i;

  vertex_t * verts = malloc(obj.vert_count * sizeof(vertex_t));
  for (i = 0; i < obj.vert_count; i++)
    verts[i] = translate_vert(scale(rotate_vert(obj.vertecies[i], obj.rotation), obj.scale),obj.pos);

  vertex_t * pverts = malloc(plane.vert_count * sizeof(vertex_t));
  for (i = 0; i < plane.vert_count; i++)
    pverts[i] = translate_vert(scale(rotate_vert(plane.vertecies[i], plane.rotation), plane.scale),plane.pos);

  tri_count = 0;

  for (i = 0; i < obj.tri_count; i++) {
    if (dot_prod(vec_normal(verts[obj.tris[i].p1].vec, verts[obj.tris[i].p2].vec, verts[obj.tris[i].p3].vec),
        vec_sub(verts[obj.tris[i].p1].vec, settings.v_cam)) > 0.0f) {
      projected_mesh[tri_count] = (settings.smooth) ?
        proj_smooth_triangle((triangle_t){verts[obj.tris[i].p1],verts[obj.tris[i].p2],verts[obj.tris[i].p3]}, settings.proj_mat, settings.v_light) :
        proj_sharp_triangle((triangle_t){verts[obj.tris[i].p1],verts[obj.tris[i].p2],verts[obj.tris[i].p3]}, settings.proj_mat, settings.v_light);
      tri_count++;
    }
  }

  for (i = 0; i < plane.tri_count; i++) {
    if (dot_prod(vec_normal(pverts[plane.tris[i].p1].vec, pverts[plane.tris[i].p2].vec, pverts[plane.tris[i].p3].vec),
        vec_sub(pverts[plane.tris[i].p1].vec, settings.v_cam)) > 0.0f) {
      projected_mesh[tri_count] = (settings.smooth) ?
        proj_smooth_triangle((triangle_t){pverts[plane.tris[i].p1],pverts[plane.tris[i].p2],pverts[plane.tris[i].p3]}, settings.proj_mat, settings.v_light) :
        proj_sharp_triangle((triangle_t){pverts[plane.tris[i].p1],pverts[plane.tris[i].p2],pverts[plane.tris[i].p3]}, settings.proj_mat, settings.v_light);
      tri_count++;
    }
  }
  free(verts);
  free(pverts);
  sort_mesh(projected_mesh, 0, tri_count-1);
  for(int i = tri_count-1; i >= 0; i--) {
    if (settings.wframe) {
      SDL_SetRenderDrawColor(renderer, 0, 244, 0, 1);
      draw_triangle(renderer, &projected_mesh[i].p1.vec, &projected_mesh[i].p2.vec, &projected_mesh[i].p3.vec);
    } else {
      raster_triangle(renderer, &projected_mesh[i].p1, &projected_mesh[i].p2, &projected_mesh[i].p3,
        projected_mesh[i].p1.light, projected_mesh[i].p2.light, projected_mesh[i].p3.light);
    }
  }
}

void main_loop(SDL_Window * window, SDL_Renderer * renderer,
               SDL_Event * event,   object_t objedwct, _object_t plane, _object_t object)
{
  bool quit = false;

    /*projection matrix constants */
  const double NEAR         = 0.1f;
  const double FAR          = 1000.0f;
  const double FOV          = 50.0f;
  const double ASPECT_RATIO = 1200.0f / 2000.0f;

  settings_t settings = (settings_t) {true, false, true,
    (vec3d_t){ 0.0, 0.0, 0.0 }, (vec3d_t){ -0.7, 0.7, 0.7 },
    projection_matrix(NEAR, FAR, FOV, ASPECT_RATIO)
  };

  /* object color */
  const color_t color = (color_t){38, 192, 183};

  /* previous mouse position */
  float pmx = 0.0f;
  float pmy = 0.0f;

  /* if mouse buttons are pressed */
  bool MOUSE1 = false;
  bool MOUSE2 = false;
  bool SHIFT = false;

  bool repaint = true;
  double offset = 0.0;
  while (!quit) {
    //color_by_height(&object, offset);
    //offset += 0.5;
    if (repaint) {
      SDL_SetRenderDrawColor(renderer, 254, 228, 194, 1);
      SDL_RenderClear(renderer);

      //render_mesh(renderer, plane, settings);
      render_object(renderer, object, plane, settings);
      //render_mesh(renderer, object, plane, settings);
      //object_t mo = obj_to_meshobject(test_obj);
      //free(object.mesh);
      //object.mesh = mo.mesh;
      //render_mesh(renderer, mo, plane, settings);
      //free(mo.mesh);
      SDL_RenderPresent(renderer);
      repaint = false;
    }

    /**
     * Event handling
     **/
    while (SDL_PollEvent(event)) {
      switch (event->type) {
        case SDL_MOUSEBUTTONDOWN: switch (event->button.button) {
            case SDL_BUTTON_LEFT:  MOUSE1 = true; break;
            case SDL_BUTTON_RIGHT: MOUSE2 = true; break;
            default:               break;
          } break;
        case SDL_MOUSEBUTTONUP: switch (event->button.button) {
            case SDL_BUTTON_LEFT:  MOUSE1 = false; break;
            case SDL_BUTTON_RIGHT: MOUSE2 = false; break;
            default:               break;
          } break;
        case SDL_MOUSEMOTION:
          if (MOUSE1 || MOUSE2) {
            if(SHIFT) {
              object.rotation.y -= (event->motion.x - pmx)/300.0;
              object.rotation.x += (event->motion.y - pmy)/300.0;
            }
            else {
              object.pos.x += (event->motion.x - pmx);
              object.pos.y += (event->motion.y - pmy);
            }
            repaint = true;
          }
          pmx = event->motion.x;
          pmy = event->motion.y;
          break;
        case SDL_MOUSEWHEEL:
          if(event->wheel.y > 0)      object.pos.z += 10;
          else if(event->wheel.y < 0) object.pos.z -= 10;
          repaint = true;
          break;
        case SDL_KEYDOWN: switch (event->key.keysym.sym) {
            case SDLK_SPACE:  settings.smooth = !settings.smooth; break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT: SHIFT = true;     break;
            case SDLK_w:      settings.wframe = !settings.wframe; break;
            default:                            break;
          } repaint = true;   break;
        case SDL_KEYUP:   switch (event->key.keysym.sym) {
            case SDLK_LSHIFT:
            case SDLK_RSHIFT: SHIFT = false; break;
            default:                         break;
          } repaint = true;   break;
        case SDL_QUIT: quit = true; break;
      }
    }
  }
}

int main(int argc, char * args[]) {

  if (argc < 2) {
    log_error("invalid argument count, enter a file to render");
    return -1;
  }
  if (!strcmp(args[1], "--help")) {
    printf("[\033[33;1;4mUsage\033[0m]\n%s <filename>   takes a .obj file to render, obj file can only contain triangles\n -v for verbose output", args[0]);
    return 0;
  }
  if (!strcmp(args[1], "-v") && argc == 2) {
    log_error("invalid argument, enter a file to render");
    return -1;
  }
  bool verbose = ( argc == 3 && (!strcmp(args[1], "-v") || !strcmp(args[2], "-v")) );

  SDL_Window   * window   = NULL;
  SDL_Renderer * renderer = NULL;
  SDL_Event      event;
  obj_t        * objp      = NULL;
  object_t       object;
  _object_t       plane;
  _object_t test_obj;

  objp = read_obj(args[1]);
  if (verbose) print_obj(objp);
  printf("controlls:\nW: wireframe\nspace: smooth shading\nmouse 1: move\nmouse1 + shift: rotate\nscroll wheel: zoom");

  //icos = malloc(objlen * sizeof(triangle_t));

  //

  object = new_object(get_triangles(objp), objp->nfaces);
  object.scale = 100.0;
  object.pos = (vec3d_t){ 0.0, 0.0, -1000.0 };
  color_by_height(&object, 0.0);

  if(convert_object(&test_obj, objp)) return -1;
  test_obj.scale = -100.0;
  test_obj.pos = (vec3d_t){ 0.0, 0.0, 1000.0 };
  _object_t cpy = shallow_copy_object(test_obj);
  print_object(cpy);
  shallow_free_obj(cpy);

  plane = _create_plane(100, 90);
  //randomize(&plane);
  plane.scale = -100.0;
  plane.pos = (vec3d_t){ 5000.0, 700.0, 10000.0 };

  init_window(&window, &renderer);


  main_loop(window, renderer, &event, object, plane, test_obj);
  free(object.mesh);
  free(objp);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  exit(0);
}
