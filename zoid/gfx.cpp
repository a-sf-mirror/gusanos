#include "gfx.h"
#include "text.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loadpng.h"

//Load image depending on format
BITMAP* load_image(const char* tmp, RGB* palette)
{
  char frmt[4];
  strncpy(frmt, &tmp[strlen(tmp) - 3], 3); frmt[3] = '\0';
  char *ufrmt = ucase(frmt);
  BITMAP* ret;
  if (strcmp(ufrmt, "BMP") == 0)
    {
      ret = load_bmp(tmp,palette);
    }
  else if (strcmp(ufrmt, "PNG") == 0)
    {
      ret = load_png(tmp, palette);
    }
  free(ufrmt);
  return ret;
}
