
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

#define WIDTH  2000
#define HEIGHT 1200

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
void main_loop(SDL_Window * window, SDL_Renderer * renderer,
                        SDL_Event * event, triangle_t * obj, int objlen)
{
  bool quit = false;

    /*projection matrix constants */
  const double NEAR         = 0.1f;
  const double FAR          = 1000.0f;
  const double FOV          = 110.0f;
  const double ASPECT_RATIO = 1200.0f / 2000.0f;

  const matrix4x4_t proj_mat = projection_matrix(NEAR, FAR, FOV, ASPECT_RATIO);

    /* position, scale, and rotation */
  const double  size = 100.0f;
  vec3d_t pos  = (vec3d_t){ 0.0, 0.0, -1000.0 };

  const vec3d_t v_cam   = (vec3d_t){ 0.0, 0.0, 0.0 };
  const vec3d_t v_light = (vec3d_t){ -0.7, 0.7, 0.7 };

  /* object color */
  const color_t color = (color_t){38, 192, 183};

  float x_ang = 0.0f;
  float y_ang = 0.0f;

  /* previous mouse position */
  float pmx = 0.0f;
  float pmy = 0.0f;

  /* if mouse buttons are pressed */
  bool MOUSE1 = false;
  bool MOUSE2 = false;
  bool SHIFT = false;

  bool repaint = true;
  bool smooth = true;
  bool wframe = false;

  //print_mat(proj_mat);

  while (!quit) {
    if (repaint) {
      SDL_SetRenderDrawColor(renderer, 254, 228, 194, 1);
      SDL_RenderClear(renderer);

      triangle_t * rot_mesh = malloc(objlen * sizeof(triangle_t));

      for (size_t i = 0; i < objlen; i++)
        rot_mesh[i] = rotate_tri(obj[i], y_ang, x_ang);

      sort_mesh(rot_mesh, 0, objlen-1);

      for (size_t i = 0; i < objlen; i++) {

        triangle_t tri = scale_tri(rot_mesh[i], size);
        tri = translate_tri(tri, pos);
        vec3d_t norm_face = normal(tri);

        if (dot_prod(norm_face, vec_sub(tri.p1.vec, v_cam)) < 0.0f) {
          tri = project_tri(tri, proj_mat);
          scale_to_window(&tri, WIDTH, HEIGHT);
          if (smooth) {
            /* individual lighting compinents for each corner */
            float la = dot_prod(rot_mesh[i].p1.normal, v_light);
            float lb = dot_prod(rot_mesh[i].p2.normal, v_light);
            float lc = dot_prod(rot_mesh[i].p3.normal, v_light);
            if (la > 1.0) la = 1.0; if (lb > 1.0) lb = 1.0; if (lc > 1.0) lc = 1.0;

            raster_triangle(renderer, &tri.p1.vec, &tri.p2.vec, &tri.p3.vec, la, lb, lc, color);
          }
          else {
            float lf = dot_prod(norm_face, v_light);
            if (lf > 1.0) lf = 1.0;
            raster_triangle(renderer, &tri.p1.vec, &tri.p2.vec, &tri.p3.vec, lf, lf, lf, color);
          }
          if (wframe) {
            SDL_SetRenderDrawColor(renderer, 0, 244, 0, 1);
            draw_triangle(renderer, &tri.p1.vec, &tri.p2.vec, &tri.p3.vec);
          }
        }
      }
      free(rot_mesh);
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
              x_ang -= (event->motion.x - pmx)/300.0;
              y_ang += (event->motion.y - pmy)/300.0;
            }
            else {
              pos.x += -(event->motion.x - pmx);
              pos.y += -(event->motion.y - pmy);
            }
            repaint = true;
          }
          pmx = event->motion.x;
          pmy = event->motion.y;
          break;
        case SDL_MOUSEWHEEL:
          if(event->wheel.y > 0)      pos.z += 10;
          else if(event->wheel.y < 0) pos.z -= 10;
          repaint = true;
          break;
        case SDL_KEYDOWN: switch (event->key.keysym.sym) {
            case SDLK_SPACE:  smooth = !smooth; break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT: SHIFT = true;     break;
            case SDLK_w:      wframe = !wframe; break;
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
  obj_t        * obj      = NULL;
  triangle_t   * icos     = NULL;
  int            objlen;

  obj = read_obj(args[1]);
  if (verbose) print_obj(obj);
  printf("controlls:\nW: wireframe\nspace: smooth shading\nmouse 1: move\nmouse1 + shift: rotate\nscroll wheel: zoom");
  objlen = obj->nfaces;
  //icos = malloc(objlen * sizeof(triangle_t));

  icos = get_triangles(obj);


  init_window(&window, &renderer);


  main_loop(window, renderer, &event, icos, objlen);
  free(icos);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  exit(0);
}
