#ifndef _RASTER_H_
#define _RASTER_H_

#include <sdl/SDL.h>
#include "vector.h"
#include "color.h"
#include "vertex.h"

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////

void draw_triangle(SDL_Renderer * rndr, const vec3d_t * a, const vec3d_t * b, const vec3d_t * c);
void raster_triangle(SDL_Renderer * rndr, const vertex_t * a, const vertex_t * b,
              const vertex_t * c,         const float la,
              const float lb,             const float lc);
#endif /* _RASTER_H_ */
