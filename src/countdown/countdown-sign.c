/** \file
 * Test the matrix LCD PRU firmware with a multi-hue rainbow.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include "ledscape.h"

const int width = 32;
const int height = 32;

extern const uint16_t font[][16];

static int
font_write(
	uint32_t * const buf,
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
			y += 16; // width;
			continue;
		}

		const uint16_t * ch = font[(uint8_t) c];
		int max_width = 0;

		if (c == ' ' || c == '.')
			max_width = 3;
		else
		for (int h = 0 ; h < 16 ; h++)
		{
			int width = 0;
			uint16_t row = ch[h] >> 2;
			while (row)
			{
				row >>= 1;
				width++;
			}

			if (width > max_width)
				max_width = width;
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
				if (x + j >= width || x + j < 0)
					continue;
*/
				if (ox >= width)
					continue;

				// wrap in x
				if (ox < 0)
					ox += width;

				if (y + h >= height || y + h < 0)
					continue;

				uint8_t * pix = (uint8_t*) &buf[(y+h)*width + ox];
			       	pix[0] = pixel_color >> 16;
			       	pix[1] = pixel_color >>  8;
			       	pix[2] = pixel_color >>  0;
			}
		}

		x += max_width;
	}

	return x;
}


int
main(
	int argc,
	char ** argv
)
{
	ledscape_config_t * config = &ledscape_matrix_default;
	if (argc > 1)
	{
		config = ledscape_config(argv[1]);
		if (!config)
			return EXIT_FAILURE;
	}

	ledscape_t * const leds = ledscape_init(config, 0);

	printf("init done\n");

	//time_t start_time = time(NULL);
	time_t now = time(NULL);
	unsigned last_i = 0;

	unsigned i = 0;
	uint32_t * const p = calloc(width*height, 4);
	int scroll_x = width;

	struct tm *last_time_local;
	char time_buffer[8]; // HH:MM:SS


	while (1)
	{
		last_time_local = localtime(&now);
		// hours in reddish color
		strftime(time_buffer, 8, "%H", last_time_local);
		font_write(p, 0xF21DC4, 0, 0, time_buffer);
		// minutes in blueish color
		strftime(time_buffer, 8, "%M", last_time_local);
		font_write(p, 0x45A2B0, width/2, 0, time_buffer);
		// seconds in dark blue color
		strftime(time_buffer, 8, "%S", last_time_local);
		font_write(p, 0x0F23D9, 0, height/2, time_buffer);
//		font_write(p, 0xFF0000, 11, 0, "!");
//		font_write(p, 0x00FF00, 224, 0, "8min");

		int end_x = font_write(p, 0xFF4000, scroll_x, 16, argc > 2 ? argv[2] : "");
		if (end_x <= 0)
			scroll_x = width;
		else
			scroll_x--;

		ledscape_draw(leds, p);
		usleep(20000);

		now = time(NULL);
	}

	ledscape_close(leds);

	return EXIT_SUCCESS;
}
