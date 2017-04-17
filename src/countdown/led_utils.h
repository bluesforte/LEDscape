#include <stdint.h>

extern int disp_img(
	uint32_t * const buf,
	int buf_width,
	int buf_height,
	const int x0,
	const int y0,
	const int img_width,
	const int img_height,
	const char * filename
);

uint32_t* load_png(
	const char * filename
	int *width,
	int *height
);

void unload_png(uint32_t * png_data);

int disp_img2(uint32_t * const buf, int buf_width, int buf_height, int x0, int y0, const uint32_t * const image, int img_width, int img_height);

extern int font_write(
  uint32_t * const buf,
  int buf_width,
  int buf_height,
  const uint32_t color,
  const int x0,
  const int y0,
  const char * s
);
