#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <zlib.h>
#include <png.h>

#include "ledscape.h"
#include "font.h"
#include "led_utils.h"

#define ERROR -1

// FIXME use structure instead of passing all the info into every function
/*typedef struct ledscape_buffer_t {
	int width;
	int height;
	uint32_t *frame;
} ledscape_buffer_t;
*/

int disp_img(
	uint32_t * const buf,
	int buf_width,
	int buf_height,
	const int x0,
	const int y0,
	const int img_width,
	const int img_height,
	const char * filename
)
{
	FILE * const fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "%s: unable to open\n", filename);
		return ERROR;
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
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_STRIP_16  /*png_transforms*/, NULL);

	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr); //row_pointers is array of length = png.height

	int png_width = png_get_image_width(png_ptr, info_ptr);
	int png_height = png_get_image_height(png_ptr, info_ptr);

	int color_depth = png_get_bit_depth(png_ptr, info_ptr);
	int color_type = png_get_color_type(png_ptr, info_ptr);
	int ilace_type = png_get_interlace_type(png_ptr, info_ptr);
	int channels = png_get_channels(png_ptr, info_ptr);
	int rbytes = png_get_rowbytes(png_ptr, info_ptr);

	printf("loaded png (w:%d,h:%d), depth:%d ctype:%d ilace_type:%d channels:%d, rbytes:%d\n", png_width, png_height, color_depth, color_type, ilace_type, channels, rbytes);
	//printf("CTYPEGRAY:%d GRAY_ALPHA:%d PALETTE:%d RGB:%d RGBA:%d", PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA);

	//int xstride = (png_width > img_width) ? png_width/img_width : 1;
	//int ystride = (png_height > img_height) ? png_height/img_height : 1;
	int xstride = 1;
	int ystride = 1;
	for (int y=0; y < buf_height; y ++) {
		if (y0+y >= png_height)
			continue;

		png_byte *row = row_pointers[y0+y];

		for (int x=0; x < buf_width; x ++) {
			if (x0+x >= png_width)
				continue;

			png_byte *png_pix = &(row[(x0 + x)*3]);
			uint8_t * pix = (uint8_t*) &buf[y*buf_width + x];
						// PNG stores in RGB format, pixels are in BGR format
						pix[0] = png_pix[2];
						pix[1] = png_pix[1];
						pix[2] = png_pix[0];
		}
	}

	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

	return 0;
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
				if (ox < 0) {
					ox %= buf_width;
					ox += buf_width;
				}

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
