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
#include "led_utils.h"

const int width = 32;
const int height = 32;

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
	//width = config->width;
	//height = config->height;

	ledscape_t * const leds = ledscape_init(config, 0);

	printf("init done\n");

	//time_t start_time = time(NULL);
	time_t now;
	unsigned last_i = 0;

	unsigned i = 0;
	uint32_t * const p = calloc(width*height, 4);

	struct tm *time_local;
	const struct tm *time_target;
	char time_buffer[8]; // HH:MM:SS

	char * img_filename = (argc > 2) ? argv[2] : "/home/harry/src/LEDscape/src/countdown/whirlyball.png";
	int img_width, img_height;

	int32_t *img = load_png(img_filename, &img_width, &img_height);

	while (1)
	{
		now = time(NULL);
		time_local = localtime(&now);
/*
		// hours in reddish color
		strftime(time_buffer, 8, "%H: ", last_time_local);
		font_write(p, width, height, 0xF21DC4, 0, 0, time_buffer);
		// minutes in blueish color
		strftime(time_buffer, 8, "%M", last_time_local);
		font_write(p, width, height, 0x45A2B0, width/2, 0, time_buffer);
		// seconds in dark blue color
		strftime(time_buffer, 8, "%S", last_time_local);
		font_write(p, width, height, 0x0F23D9, 0, height/2, time_buffer);
*/
		//for (int xscroll = 0; xscroll < 75; xscroll++) {
		//	disp_img( p, width, height, xscroll, 10, 32, 25, argv[2] );
		/*int end_x = font_write(p, width, height, 0xFF4000, scroll_x, 16, argc > 2 ? argv[2] : "");
		if (end_x <= 0)
			scroll_x = width;
		else
			scroll_x--;*/

			disp_img2(p, width, height, 1,1, img, img_width, img_height);

			ledscape_draw(leds, p);
			usleep(100000);
		}

	}

	ledscape_close(leds);

	return EXIT_SUCCESS;
}
