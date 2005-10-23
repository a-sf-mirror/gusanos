#ifndef DEDSERV

#include "blitters.h"
#include "macros.h"

#include <algorithm>

namespace Blitters
{
	
void drawSprite_multsec_32_with_8(BITMAP* where, BITMAP* from, BITMAP* secondary, int x, int y, int sx, int sy, int cutl, int cutt, int cutr, int cutb)
{
	typedef Pixel32 pixel_t_1;
	typedef Pixel8 pixel_t_sec;
	
	if(bitmap_color_depth(from) != 32
	|| bitmap_color_depth(secondary) != 8)
		return;

	CLIP_SPRITE_REGION_SEC();
	
	SPRITE_Y_LOOP_SEC(
		SPRITE_X_LOOP_SEC(
			*dest = scaleColor_32(*src, *sec);
		)
	)

}

void drawSprite_mult_32_with_8(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb)
{
	typedef Pixel32 pixel_t_dest;
	typedef Pixel8 pixel_t_src;
	
	if(bitmap_color_depth(where) != 32
	|| bitmap_color_depth(from) != 8)
		return;

	CLIP_SPRITE_REGION();
	
	SPRITE_Y_LOOP(
		SPRITE_X_LOOP_T(
			*dest = scaleColor_32(*dest, *src);
		)
	)

}

} // namespace Blitters

#endif