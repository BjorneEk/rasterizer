#ifndef _COLOR_H_
#define _COLOR_H_


////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////
#include <util/util.h>

typedef struct color
{
	float r, g, b;
} color_t;

#define COLOR_MAX 255

#define NULL_COLOR  (color_t){0, 0, 0}
#define COLOR_RED   (color_t){COLOR_MAX, 0, 0}
#define COLOR_GREEN (color_t){0, COLOR_MAX, 0}
#define COLOR_BLUE  (color_t){0, 0, COLOR_MAX}
#define COLOR_BLACK (color_t){0, 0, 0}
#define COLOR_WHITE (color_t){COLOR_MAX, COLOR_MAX, COLOR_MAX}

color_t from_angle(float a);
color_t with_brightness(color_t color, float b);

void clr_add(color_t * c, float v);


#endif /* _COLOR_H_ */
