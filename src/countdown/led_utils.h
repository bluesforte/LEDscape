#include <stdint.h>

#include "font.h"

static int font_write(
  uint32_t * const buf,
  int width,
  int height,
  const uint32_t color,
  const int x0,
  const int y0,
  const char * s
);
