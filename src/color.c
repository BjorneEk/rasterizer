
#include "include/color.h"
#include <math.h>

color_t from_angle(float a)
{
  return (color_t) {
    .r = fabsf(sin(0.3f * (a) + 0.0f)) * 255.0,
    .g = fabsf(sin(0.3f * (a) + 2.0f)) * 255.0,
    .b = fabsf(sin(0.3f * (a) + 4.0f)) * 255.0
  };
}
color_t with_brightness(color_t color, float b) {
  return (color_t)
  {
    b * color.r,
    b * color.g,
    b * color.b,
  };
}

void clr_add(color_t * c, float v)
{
  if(c->r + v > 0.0f && c->r + v <= 255.0f) c->r += v;
  if(c->g + v > 0.0f && c->g + v <= 255.0f) c->g += v;
  if(c->b + v > 0.0f && c->b + v <= 255.0f) c->b += v;
}
