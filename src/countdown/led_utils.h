#include <stdint.h>

extern int font_write(
  uint32_t * const buf,
  int buf_width,
  int buf_height,
  const uint32_t color,
  const int x0,
  const int y0,
  const char * s
);