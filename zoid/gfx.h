#ifndef __gfx_h__
#define __gfx_h__

#include <allegro.h>

BITMAP* load_image(const char *, RGB*);

BITMAP* load_image_with_no_ext(const char *, RGB*);

#endif
