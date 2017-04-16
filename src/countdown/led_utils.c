#include <stdint.h>
#include <png.h>

#include "ledscape.h"
#include "font.h"
#include "led_utils.h"

// FIXME use structure instead of passing all the info into every function
/*typedef struct ledscape_buffer_t {
	int width;
	int height;
	uint32_t *frame;
} ledscape_buffer_t;
*/

int disp_img(uint32_t * const buf, int buf_width, int buf_height, const int x0, const int y0, const int img_width, const int img_height, const char * filename) {
	FILE * const fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "%s: unable to open\n", filename);
		return NULL;
	}

	png_structp png_ptr = png_create_read_struct(
        	PNG_LIBPNG_VER_STRING,
					(png_voidp)NULL /*user_error_ptr*/,
					NULL /*user_error_fn*/,
					NULL /*user_warning_fn*/
				);
  if (!png_ptr)
     return (ERROR);

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
     png_destroy_read_struct(&png_ptr,
         (png_infopp)NULL, (png_infopp)NULL);
     return (ERROR);
  }

	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_SCALE_16/*png_transforms*/, NULL)

	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr); //row_pointers is array of length = png.height

	int img_width = png_get_image_width(png_ptr, info_ptr);
  int img_height = png_get_image_height(png_ptr, info_ptr);

	
}

int font_write(
	uint32_t * const buf,
	int buf_width,
	int buf_height,
	const uint32_t color,
	const int x0,
	const int y0,
	const char * s
)
{
	int x = x0;
	int y = y0;

	while (1)
	{
		char c = *s++;
		if (!c)
			break;

		if (c == '\n')
		{
			x = x0;
			y += 16; // buf_width;
			continue;
		}

		const uint16_t * ch = font[(uint8_t) c];
		int max_width = 0;

		if (c == ' ' || c == '.')
			max_width = 3;
		else
		for (int h = 0 ; h < 16 ; h++)
		{
			int buf_width = 0;
			uint16_t row = ch[h] >> 2;
			while (row)
			{
				row >>= 1;
				buf_width++;
			}

			if (buf_width > max_width)
				max_width = buf_width;
		}

		// add space after the character
		max_width++;

		for (int h = 0 ; h < 16 ; h++)
		{
			uint16_t row = ch[h] >> 2;
			for (int j = 0 ; j < max_width ; j++, row >>= 1)
			{
				uint32_t pixel_color = (row & 1) ? color : 0;
				int ox = x + j;
/*
				if (x + j >= buf_width || x + j < 0)
					continue;
*/
				if (ox >= buf_width)
					continue;

				// wrap in x
				if (ox < 0)
					ox += buf_width;

				if (y + h >= buf_height || y + h < 0)
					continue;

				uint8_t * pix = (uint8_t*) &buf[(y+h)*buf_width + ox];
			       	pix[0] = pixel_color >> 16;
			       	pix[1] = pixel_color >>  8;
			       	pix[2] = pixel_color >>  0;
			}
		}

		x += max_width;
	}

	return x;
}
