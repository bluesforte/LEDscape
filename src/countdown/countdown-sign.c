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

	int i;
	//time_t start_time = time(NULL);
	time_t now;
	struct tm *time_local;

	uint32_t * const p = calloc(width*height, 4);

	time_t target = time(NULL);
	struct tm *time_target = localtime(&target);

	// set datetime of 4/20 @ 7am
	time_target->tm_mon = 04; time_target->tm_mday = 20;
	time_target->tm_hour = 07; time_target->tm_min = 00; time_target->tm_sec = 00;
	time_t tt =  mktime(time_target);

	char time_buffer[8]; // HH:MM:SS

	char * img_filename = (argc > 2) ? argv[2] : "/home/harry/src/LEDscape/src/countdown/whirlyball.png";
	int img_width, img_height;

	//uint32_t *img = load_png(img_filename, &img_width, &img_height);

	while (1)
	{
		// display timer for about 10 seconds
		for (i = 0; i < 10; i++) {
			time(&now);
			//time_local = localtime(&now);
			int seconds = (int)difftime(tt, now);
			// days in dark green color
			sprintf(time_buffer, "%02d", seconds / (3600 * 24) );
			font_write(p, width, height, 0x920FFF, 0, 0, time_buffer);

			// hours in reddish color
			sprintf(time_buffer, "%02d", (seconds / 3600) % 24 );
			font_write(p, width, height, 0xF21DC4, width/2, 0, time_buffer);
			// minutes in blueish color
			//strftime(time_buffer, 8, "%M", time_local);
			sprintf(time_buffer, "%02d", (seconds / 60) % 60 );
			font_write(p, width, height, 0x45A2B0, 0, height/2, time_buffer);
			// seconds in dark blue color
			//strftime(time_buffer, 8, "%S", time_local);
			sprintf(time_buffer, "%02d", seconds % 60 );
			font_write(p, width, height, 0x0F23D9, width/2, height/2, time_buffer);

			ledscape_draw(leds, p);
			usleep(1000000);
		}

		// scroll the picture
		for (int xscroll = 0; xscroll < 42; xscroll++) {
			//disp_img2(p, width, height, 1,1, img, img_width, img_height);
			disp_img( p, width, height, xscroll, 10, 32, 32, img_filename );
			ledscape_draw(leds, p);
			usleep(200000);
		}

		// sleep an extra second for fun
		usleep(1000000);

		//clean up buffer
		for (i = 0; i < width *height * 4; i++) {
			p[i] = 0;
		}

	}

	ledscape_close(leds);

	return EXIT_SUCCESS;
}
