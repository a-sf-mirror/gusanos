#ifndef RENDER_H
#define RENDER_H

#include <ctype.h>
#include <allegro.h>
#ifdef WINDOWS
#include "winalleg.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "engine.h"

#ifdef AAFBLEND
#include <fblend.h>
#endif

#ifdef AA2XSAI
extern "C" {
#include "2xsai.h"
}
#endif

extern BITMAP* screen_buffer;

bool CanBeSeen(int x, int y, int w, int h);

#endif /* RENDER_H */
