
#include "include/raster.h"
#include <util/util.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////

#define WIDTH  2000
#define HEIGHT 1200

static i32_t max(const i32_t a, const i32_t b)                 __attribute__((always_inline));
static i32_t max3(const i32_t a, const i32_t b, const i32_t c) __attribute__((always_inline));
static i32_t min(const i32_t a, const i32_t b)                 __attribute__((always_inline));
static i32_t min3(const i32_t a, const i32_t b, const i32_t c) __attribute__((always_inline));
static float edge_function(const vec3d_t * restrict a,
                  const vec3d_t * restrict b,
                  const float cx, const float cy)          __attribute__((always_inline));


void draw_triangle(SDL_Renderer * rndr, const vec3d_t * a, const vec3d_t * b, const vec3d_t * c)
{
  SDL_RenderDrawLine(rndr, (int)(a->x), (int)(a->y), (int)(b->x), (int)(b->y));
  SDL_RenderDrawLine(rndr, (int)(b->x), (int)(b->y), (int)(c->x), (int)(c->y));
  SDL_RenderDrawLine(rndr, (int)(c->x), (int)(c->y), (int)(a->x), (int)(a->y));
}

void raster_triangle(SDL_Renderer * rndr, const vertex_t * a, const vertex_t * b,
                                          const vertex_t * c, const float la,
                                          const float lb,     const float lc)
{
  i32_t min_x, min_y;
  i32_t max_x, max_y;

  // CCompute triangle bounding box and clip against screen bounds
  min_x = max(min3(a->vec.x, b->vec.x, c->vec.x), 0);
  min_y = max(min3(a->vec.y, b->vec.y, c->vec.y), 0);
  max_x = min(max3(a->vec.x, b->vec.x, c->vec.x), WIDTH - 1);
  max_y = min(max3(a->vec.y, b->vec.y, c->vec.y), HEIGHT - 1);

  // rasterize triangle
  for (i32_t y = min_y; y <= max_y; y++) {
    for (i32_t x = min_x; x <= max_x; x++) {
      // Determine barycentric coordinates
      float w0 = edge_function(&b->vec, &c->vec, (float)x, (float)y);
      float w1 = edge_function(&c->vec, &a->vec, (float)x, (float)y);
      float w2 = edge_function(&a->vec, &b->vec, (float)x, (float)y);

      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0)
      {
        w0 /= edge_function(&b->vec, &c->vec, a->vec.x, a->vec.y);
        w1 /= edge_function(&c->vec, &a->vec, b->vec.x, b->vec.y);
        w2 /= edge_function(&a->vec, &b->vec, c->vec.x, c->vec.y);

        float red   = (w0 * (la * a->color.r)) + (w1 * (lb * b->color.r)) + (w2 * (lc * c->color.r));
        float green = (w0 * (la * a->color.g)) + (w1 * (lb * b->color.g)) + (w2 * (lc * c->color.g));
        float blue  = (w0 * (la * a->color.b)) + (w1 * (lb * b->color.b)) + (w2 * (lc * c->color.b));
        SDL_SetRenderDrawColor(rndr, red, green, blue, 1);
        SDL_RenderDrawPoint(rndr, x, y);
      }
    }
  }
}

static float edge_function(const vec3d_t * restrict a,
                  const vec3d_t * restrict b, const float cx, const float cy)
{ return (b->x - a->x) * (cy - a->y) - (b->y - a->y) * (cx - a->x); }

static i32_t max(const i32_t a, const i32_t b)
{ return (b > a) ? b : a; }

static i32_t max3(const i32_t a, const i32_t b, const i32_t c)
{ return (b > a) ? ((c > b) ? c : b) : ((c > a) ? c : a); }

static i32_t min(const i32_t a, const i32_t b)
{ return (b < a) ? b : a; }

static i32_t min3(const i32_t a, const i32_t b, const i32_t c)
{ return (b < a) ? ((c < b) ? c : b) : ((c < a) ? c : a); }
