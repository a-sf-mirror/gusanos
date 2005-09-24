#ifndef OMFG_BLITTERS_BLITTERS_H
#define OMFG_BLITTERS_BLITTERS_H

#ifdef DEDSERV
#error "Can't use this in dedicated server"
#endif //DEDSERV

#include <allegro.h>
#include "types.h"
//#include "mmx.h"

#define HAS_MMX (cpu_capabilities & CPU_MMX)
#define HAS_SSE (cpu_capabilities & CPU_SSE)

//#define HAS_MMX (false)
//#define HAS_SSE (false)


#define FOR_MMX(x_) if(HAS_MMX) { x_ }

namespace Blitters
{

const Pixel maskcolor_32 = 0xFF00FF;
const Pixel maskcolor_16 = 0xF81F;

/*
	Naming:
	
	function - bitdepth [ - parallelism]
	
	e.g.:
	scaleColor_32
	
	defaults:
		parallelism = 1
*/

inline Pixel scaleColor_32(Pixel color, int fact)
{
	Pixel temp1 = color & 0xFF00FF;
	Pixel temp2 = color & 0x00FF00;
	
	temp1 = ((temp1 * fact) >> 8) & 0xFF00FF;
	temp2 = ((temp2 * fact) >> 8) & 0x00FF00;
	
	return temp1 | temp2;
}

inline Pixel scaleColor_16(Pixel color, int fact)
{
	color = ((color << 16) | color) & 0x7E0F81F;

	color = ((color * fact + 0x2008010) >> 5) & 0x7E0F81F;
	
	return (color | (color >> 16)) & 0xFFFF;
}

inline Pixel scaleColor_16_2(Pixel color, int fact)
{
	Pixel color1 = color & 0x7E0F81F;
	color1 = ((color1 * fact + 0x2008010) >> 5) & 0x7E0F81F;
	
	Pixel color2 = (color & 0xF81F07E0) >> 5;
	color2 = (color2 * fact + 0x4008010) & 0xF81F07E0;
	
	return color1 | color2;
}

inline Pixel convertColor_32_to_16(Pixel color)
{
	Pixel r = color & 0xF80000;
	Pixel g = color & 0x00FC00;
	Pixel b = color & 0x0000F8;
	return (r >> 8) | (g >> 5) | (b >> 3);
}

// color must contain only one pixel
inline Pixel duplicateColor_16(Pixel color)
{
	return color | (color << 16);
}

inline Pixel packColors_16(Pixel color1, Pixel color2)
{
	return color1 | (color2 << 16);
}

inline Pixel32 addColorsCrude_32(Pixel color1, Pixel color2)
{
	color1 = (color1 & 0xFEFEFF) + (color2 & 0xFEFEFF);
	Pixel32 temp1 = (color1 & 0x01010100) >> 7;
	color1 |= 0x010101 - temp1;
	return color1 & 0xFFFFFF;
}

/* Not recommended to use
inline Pixel16 addColorsCrude_16(Pixel color1, Pixel color2)
{
	color1 = (color1 & 0xF7DF) + (color2 & 0xF7DF);
	Pixel32 temp1 = (color1 & 0x10820);
	color1 |= temp1 - (temp1 >> 5);
	return color1 & 0xFFFF;
}*/

inline Pixel addColors_16_2(Pixel color1, Pixel color2)
{
	const Pixel msb = 0x84108410;
	
	Pixel msb_x = color1 & msb;
 	Pixel msb_y = color2 & msb;

 	Pixel sum = (color1 & ~msb) + (color2 & ~msb);

	Pixel p = msb_x | msb_y;
	Pixel g = msb_x & msb_y;
	Pixel c = p & sum;

	Pixel overflow = (c | g) >> 4;

	return ((msb - overflow) ^ msb) | sum | p;
}

// Does precomputation for a color in preparation for
// the three parameter version of addColors_16_2.
inline void prepareAddColors_16_2(Pixel color2, Pixel& msb_y, Pixel& color2rest)
{
	const Pixel msb = 0x84108410;
	msb_y = color2 & msb;
	color2rest = color2 & ~msb;
}

// This function can add two pixels at a time
inline Pixel addColors_16_2(Pixel color1, Pixel msb_y, Pixel color2rest)
{
	const Pixel msb = 0x84108410;
	
	Pixel msb_x = color1 & msb;

 	Pixel sum = (color1 & ~msb) + color2rest;

	Pixel p = msb_x | msb_y;
	Pixel g = msb_x & msb_y;
	Pixel c = p & sum;

	Pixel overflow = (c | g) >> 4;

	return ((msb - overflow) ^ msb) | sum | p;
}

inline Pixel addColorsAllegro_16(Pixel x, Pixel y, int n)
{
   int r = getr16(y) + getr16(x) * n / 256;
   int g = getg16(y) + getg16(x) * n / 256;
   int b = getb16(y) + getb16(x) * n / 256;

   r = r > 255 ? 255 : r;
   g = g > 255 ? 255 : g;
   b = b > 255 ? 255 : b;

   return makecol16(r, g, b);
}

inline Pixel32 blendColorsHalf_32(Pixel color1, Pixel color2)
{
	return ((color1 & 0xFEFEFE) >> 1)
	     + ((color2 & 0xFEFEFE) >> 1)
	     + (color1 & color2 & 0x010101);
}

inline Pixel32 blendColorsHalfCrude_32(Pixel color1, Pixel color2)
{
	return ((color1 & 0xFEFEFE) >> 1) + ((color2 & 0xFEFEFE) >> 1);
}

inline void prepareBlendColorsHalfCrude_32(Pixel color2, Pixel& color2halved)
{
	color2halved = ((color2 & 0xFEFEFE) >> 1);
}

inline Pixel32 blendColorsHalfCrude_32_prepared(Pixel color1, Pixel color2halved)
{
	return ((color1 & 0xFEFEFE) >> 1) + color2halved;
}

inline Pixel blendColorsFact_32(Pixel color1, Pixel color2, int fact)
{
	Pixel res = (((color2 & 0xFF00FF) - (color1 & 0xFF00FF)) * fact >> 8) + color1;
	color1 &= 0xFF00;
	color2 &= 0xFF00;
	Pixel g = ((color2 - color1) * fact >> 8) + color1;

	res &= 0xFF00FF;
	g &= 0xFF00;

	return res | g;
}

// Does precomputation for a color in preparation for
// the four parameter version of blendColorsFact_32.
inline void prepareBlendColorsFact_32(Pixel color2, Pixel& color2rb, Pixel& color2g)
{
	color2rb = color2 & 0xFF00FF;
	color2g  = color2 & 0x00FF00;
}

/*
inline Pixel32 blendColorsFact_32(Pixel32 color1, Pixel32 color2rb, Pixel32 color2g, int fact)
{
	Pixel32 temp2 = color1 & 0xFF00FF;
	Pixel32 temp1;
	temp1 = ((((color2rb - temp2) * fact) >> 8) + temp2) & 0xFF00FF;
	color1 &= 0xFF00;
	temp2 = ((((color2g - color1) * fact) >> 8) + color1) & 0xFF00;
	return temp1 | temp2;
	
	Pixel res = (((color1 & 0xFF00FF) - color2rb) * fact >> 8) + color2;
	color1 &= 0xFF00;
	Pixel g = ((color1 - color2g) * fact >> 8) + color2;

	res &= 0xFF00FF;
	g &= 0xFF00;

	return res | g;
}*/

inline Pixel32 blendColorsHalf_16_2(Pixel color1, Pixel color2)
{
	return (((color1 & 0xF7DEF7DE) >> 1)
	     + ((color2 & 0xF7DEF7DE) >> 1)
	     + (color1 & color2 & 0x08210821));
}

inline void prepareBlendColorsHalf_16_2(Pixel color2, Pixel& color2mask, Pixel& color2halved)
{
	color2mask = color2 & 0x08210821;
	color2halved = ((color2 & 0xF7DEF7DE) >> 1);
}

inline Pixel32 blendColorsHalf_16_2_prepared(Pixel color1, Pixel color2mask, Pixel color2halved)
{
	return (((color1 & 0xF7DEF7DE) >> 1)
	     + color2halved
	     + (color1 & color2mask));
}

inline Pixel32 blendColorsFact_16_2(Pixel color1, Pixel color2, int fact)
{
	Pixel temp1, temp2;
	temp2 = color2 & 0x7E0F81F;
	color2 &= 0xF81F07E0;
	temp1 = color1 & 0x7E0F81F;
	color1 &= 0xF81F07E0;

	color1 = ((((color2 >> 5) - (color1 >> 5)) * fact + 0x4008010) + color1) & 0xF81F07E0;
	color2 = ((((temp2 - temp1) * fact + 0x2008010) >> 5) + temp1) & 0x7E0F81F;

	return color1 | color2;
}

inline Pixel32 blendColorsFact_16(Pixel color1, Pixel color2, int fact)
{
	// TODO: Improve this?
	return blendColorsFact_16_2(color1, color2, fact);
}

inline void prepareBlendColorsFact_16_2(Pixel color2, Pixel& color2a, Pixel& color2b)
{
	color2a = color2 & 0x7E0F81F;
	color2b = color2 & 0xF81F07E0;
}

inline Pixel32 blendColorsFact_16_2(Pixel color1, Pixel color2a, Pixel color2b, int fact)
{
	Pixel temp1;
	temp1 = color1 & 0x7E0F81F;
	color1 &= 0xF81F07E0;

	color1  = ((((color2b >> 5) - (color1 >> 5)) * fact) + color1) & 0xF81F07E0;
	color2b = ((((color2a - temp1) * fact) >> 5) + temp1) & 0x7E0F81F;

	return color1 | color2b;
}

inline Pixel blendColorsFact_16(Pixel colors, int fact)
{
	Pixel x = ((colors & 0xFFFF) | (colors << 16)) & 0x7E0F81F;
	Pixel y = ((colors >> 16)    | (colors & 0xFFFF0000)) & 0x7E0F81F;

	Pixel result = (((x - y) * fact >> 5) + y) & 0x7E0F81F;

	return ((result & 0xFFFF) | (result >> 16));
}

inline Pixel blendColorsAllegro_16(Pixel x, Pixel y, int n)
{
   unsigned long result;

   if (n)
      n = (n + 1) / 8;

   x = ((x & 0xFFFF) | (x << 16)) & 0x7E0F81F;
   y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;

   result = ((x - y) * n / 32 + y) & 0x7E0F81F;

   return ((result & 0xFFFF) | (result >> 16));
}

inline Pixel blend_mask_16_2(Pixel dest, Pixel src)
{
	if((src & 0xFFFF) == maskcolor_16)
		src = (src & 0xFFFF0000) | (dest & 0xFFFF);
		
	if((src & 0xFFFF0000) == (maskcolor_16 << 16))
		src = (src & 0xFFFF) | (dest & 0xFFFF0000);
		
	return src;
}

inline Pixel add_mask_16_2(Pixel src)
{
	if((src & 0xFFFF) == maskcolor_16)
		src = (src & 0xFFFF0000);
		
	if((src & 0xFFFF0000) == (maskcolor_16 << 16))
		src = (src & 0xFFFF);
		
	return src;
}

/*
	Naming:
	
	function [ - filter] - bitdepth [ - parallelism] [ - variant]
	
	e.g.:
	rectfill_blend_32_mmx
	
	defaults:
		parallelism = 1
		variant = C
*/

inline void putpixel_solid_32(BITMAP* where, int x, int y, Pixel color1)
{
	((Pixel32 *)where->line[y])[x] = color1;
}

void putpixel_add_32(BITMAP* where, int x, int y, Pixel color1);
void putpixel_addFact_32(BITMAP* where, int x, int y, Pixel color1, int fact);
void putpixelwu_add_32(BITMAP* where, float x, float y, Pixel color1, int fact);
void putpixel_blendHalf_32(BITMAP* where, int x, int y, Pixel color1);
void putpixel_blend_32(BITMAP* where, int x, int y, Pixel color1, int fact);
void putpixelwu_blend_32(BITMAP* where, float x, float y, Pixel color1, int fact);

inline void putpixel_solid_16(BITMAP* where, int x, int y, Pixel color1)
{
	((Pixel16 *)where->line[y])[x] = color1;
}

void putpixel_add_16(BITMAP* where, int x, int y, Pixel color1);
void putpixel_addFact_16(BITMAP* where, int x, int y, Pixel color1, int fact);
void putpixelwu_add_16(BITMAP* where, float x, float y, Pixel color1, int fact);
void putpixel_blendHalf_16(BITMAP* where, int x, int y, Pixel color1);
void putpixel_blend_16(BITMAP* where, int x, int y, Pixel color1, int fact);
void putpixelwu_blend_16(BITMAP* where, float x, float y, Pixel color1, int fact);


void rectfill_add_16(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact);
void rectfill_blend_16(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact);

void rectfill_add_32(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact);
void rectfill_add_32_mmx(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact);
void rectfill_blend_32(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact);

bool linewu_blend(BITMAP* where, float x, float y, float destx, float desty, Pixel colour, int fact);
bool linewu_add(BITMAP* where, float x, float y, float destx, float desty, Pixel colour, int fact);

void drawSprite_add_16(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_add_16_sse(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blend_16(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blend_16_sse(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blendalpha_32_to_16(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blendtint_8_to_16(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact, int color);

void drawSprite_add_32(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_add_32_sse(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blend_32(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blend_32_sse(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blendalpha_32_to_32(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blendalpha_32_to_32_sse_amd(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact);
void drawSprite_blendtint_8_to_32(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact, int color);

} // namespace Blitters

using Blitters::linewu_blend;
using Blitters::linewu_add;


// Automatic color depth and variant selecting functions
#define SELECT(f_, x_) \
	switch(bitmap_color_depth(where)) { \
		case 16: Blitters::f_##_16 x_ ; break; \
		case 32: Blitters::f_##_32 x_ ; break; }
		
#define SELECT_SSE32(f_, x_) \
	switch(bitmap_color_depth(where)) { \
		case 16: Blitters::f_##_16 x_ ; break; \
		case 32: \
			if(HAS_SSE) Blitters::f_##_32_sse x_ ; \
			else Blitters::f_##_32 x_ ; \
		break; }
		
#define SELECT_SSE_OR_3DNOW_32(f_, x_) \
	switch(bitmap_color_depth(where)) { \
		case 16: Blitters::f_##_16 x_ ; break; \
		case 32: \
			if(HAS_SSE) Blitters::f_##_32_sse_amd x_ ; \
			else Blitters::f_##_32 x_ ; \
		break; }
		
#define SELECT_SSE(f_, x_) \
	switch(bitmap_color_depth(where)) { \
		case 16: \
			if(HAS_SSE) Blitters::f_##_16_sse x_ ; \
			else Blitters::f_##_16 x_ ; \
		break; \
		case 32: \
			if(HAS_SSE) Blitters::f_##_32_sse x_ ; \
			else Blitters::f_##_32 x_ ; \
		break; }
		
#define SELECT2(f_, a_, b_) \
	switch(bitmap_color_depth(where)) { \
		case 16: Blitters::f_##_16 b_ ; break; \
		case 32: Blitters::f_##_32 a_ ; break; }

#define CHECK_RANGE() \
	if((unsigned int)x >= (unsigned int)where->w \
	|| (unsigned int)y >= (unsigned int)where->h) \
		return
		
inline void putpixel_add(BITMAP* where, int x, int y, Pixel color1, int fact)
{
	CHECK_RANGE();
	SELECT2(putpixel_addFact, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixel_addFull(BITMAP* where, int x, int y, Pixel color1)
{
	CHECK_RANGE();
	SELECT(putpixel_add, (where, x, y, color1));
}

inline void putpixelwu_add(BITMAP* where, float x, float y, Pixel color1, int fact)
{
	//blendwu blender checks range
	SELECT2(putpixelwu_add, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixel_blendHalf(BITMAP* where, int x, int y, Pixel color1)
{
	CHECK_RANGE();
	SELECT(putpixel_blendHalf, (where, x, y, color1));
}

inline void putpixel_blend(BITMAP* where, int x, int y, Pixel color1, int fact)
{
	CHECK_RANGE();
	SELECT2(putpixel_blend, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixel_blendalpha(BITMAP* where, int x, int y, Pixel color1, int fact)
{
	CHECK_RANGE();
	SELECT2(putpixel_blend, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixelwu_blend(BITMAP* where, float x, float y, Pixel color1, int fact)
{
	//blendwu blender checks range
	SELECT2(putpixelwu_blend, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixelwu_blendalpha(BITMAP* where, float x, float y, Pixel color1, int fact)
{
	//blendwu blender checks range
	SELECT2(putpixelwu_blend, (where, x, y, color1, fact), (where, x, y, color1, (fact + 4) / 8));
}

inline void putpixel_solid(BITMAP* where, int x, int y, Pixel color1)
{
	CHECK_RANGE();
	SELECT(putpixel_solid, (where, x, y, color1));
}

inline void putpixelwu_solid(BITMAP* where, float x, float y, Pixel color1)
{
	//blendwu blender checks range
	SELECT2(putpixelwu_blend, (where, x, y, color1, 256), (where, x, y, color1, 32));
}

inline void rectfill_add(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact)
{
	SELECT(rectfill_add, (where, x1, y1, x2, y2, colour, fact));
}

inline void rectfill_blend(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact)
{
	SELECT(rectfill_blend, (where, x1, y1, x2, y2, colour, fact));
}

inline void rectfill_blendalpha(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour, int fact)
{
	SELECT(rectfill_blend, (where, x1, y1, x2, y2, colour, fact));
}

inline void rectfill_solid(BITMAP* where, int x1, int y1, int x2, int y2, Pixel colour)
{
	rectfill(where, x1, y1, x2, y2, colour); //TODO: Make own
}

inline void linewu_blendalpha(BITMAP* where, float x, float y, float destx, float desty, Pixel colour, int fact)
{
	Blitters::linewu_blend(where, x, y, destx, desty, colour, fact);
}

inline void linewu_solid(BITMAP* where, float x, float y, float destx, float desty, Pixel colour)
{
	Blitters::linewu_blend(where, x, y, destx, desty, colour, 256);
}

inline void drawSprite_add(BITMAP* where, BITMAP* from, int x, int y, int fact)
{
	SELECT(drawSprite_add, (where, from, x, y, 0, 0, 0, 0, fact));
}

inline void drawSprite_blend(BITMAP* where, BITMAP* from, int x, int y, int fact)
{
	SELECT_SSE(drawSprite_blend, (where, from, x, y, 0, 0, 0, 0, fact));
}

inline void drawSprite_blendalpha(BITMAP* where, BITMAP* from, int x, int y, int fact)
{
	SELECT_SSE_OR_3DNOW_32(drawSprite_blendalpha_32_to, (where, from, x, y, 0, 0, 0, 0, fact));
}

inline void drawSprite_blendtint(BITMAP* where, BITMAP* from, int x, int y, int fact, int color)
{
	SELECT(drawSprite_blendtint_8_to, (where, from, x, y, 0, 0, 0, 0, fact, color));
}

inline void drawSprite_solid(BITMAP* where, BITMAP* from, int x, int y)
{
	draw_sprite(where, from, x, y);
}


inline void drawSpriteCut_add(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact)
{
	SELECT(drawSprite_add, (where, from, x, y, cutl, cutt, cutr, cutb, fact));
}

inline void drawSpriteCut_blend(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact)
{
	SELECT_SSE(drawSprite_blend, (where, from, x, y, cutl, cutt, cutr, cutb, fact));
}

inline void drawSpriteCut_blendalpha(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb, int fact)
{
	SELECT_SSE_OR_3DNOW_32(drawSprite_blendalpha_32_to, (where, from, x, y, cutl, cutt, cutr, cutb, fact));
}

inline void drawSpriteCut_solid(BITMAP* where, BITMAP* from, int x, int y, int cutl, int cutt, int cutr, int cutb)
{
	masked_blit(from, where, cutl, cutt, x+cutl, y+cutt
		, from->w - (cutl + cutr)
		, from->h - (cutt + cutb));
}

#undef SELECT

#endif //OMFG_BLITTERS_BLITTERS_H
