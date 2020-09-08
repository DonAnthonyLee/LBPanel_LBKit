/* --------------------------------------------------------------------------
 *
 * Commands for LCD ST7735S
 * Copyright (C) 2018, Anthony Lee, All Rights Reserved
 *
 * This software is a freeware; it may be used and distributed according to
 * the terms of The MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * File: lcd_show.c
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char	bool;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#include <lcd_st7735s_ioctl.h>

// lcd_probe.c
extern char* lcd_st7735s_probe_default_device(void);

#define DEFAULT_COLOR		0x0000cbff

static void show_usage(void)
{
	char *dev_name = lcd_st7735s_probe_default_device();

	printf("lcd_show - Show characters on LCD\n\n");
	printf("Usage: lcd_show [-D device] x y size string [rgb_color]\n\
    device                       path of device, default value is: %s\n\
    size = 8,12,14,16,24,32,0    0 means 32x32 icon, range: 0~9.\n\
    rgb_color[B:G:R]             default value is: 0x%06x\n", (dev_name ? dev_name : "none"), DEFAULT_COLOR);

	if (dev_name != NULL)
		free(dev_name);
}

#ifdef CMD_ALL_IN_ONE
int cmd_show(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0;
	char *dev_name = NULL;
	_lcd_st7735s_draw_str_t data;
	uint32_t color = DEFAULT_COLOR;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-D") == 0) {
			if(dev_name != NULL)
				free(dev_name);
			dev_name = strdup(argv[++n]);
		} else {
			break;
		}
	}
	argc -= (--n);

	if (argc < 5 || argc > 6) {
		if (dev_name != NULL)
			free(dev_name);
		show_usage();

		exit(1);
	}

	if (dev_name == NULL)
		dev_name = lcd_st7735s_probe_default_device();

	if (dev_name == NULL || (f = open(dev_name, O_RDWR)) < 0) {
		fprintf(stderr, "No such device: %s\n", (dev_name == NULL) ? "none" : dev_name);
		if(dev_name != NULL)
			free(dev_name);

		exit(1);
	}
	free(dev_name);

	data.x = (int16_t)atoi(argv[n + 1]);
	data.y = (int16_t)atoi(argv[n + 2]);
	data.size = (uint8_t)atoi(argv[n + 3]);
	strncpy(data.str, argv[n + 4], sizeof(data.str));

	if (argc == 6)
		color = (uint32_t)strtoul(argv[n + 5], NULL, 16);

	data.color = (color & 0x000000f8) << 8; /* red */
	data.color |= (color & 0x0000fc00) >> 5; /* green */
	data.color |= (color & 0x00f80000) >> 19; /* blue */

	if ((err = ioctl(f, LCD_ST7735S_IOC_DRAW_STRING, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

