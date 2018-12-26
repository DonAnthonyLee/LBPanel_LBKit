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
 * File: lcd_clear.c
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

#define DEFAULT_DEVICE		"/dev/spi-lcd0.1"
#define DEFAULT_COLOR		0

static void show_usage(void)
{
	printf("lcd_clear - Clear LCD\n\n");
	printf("Usage: lcd_clear [-D device] x y width height [rgb_color]\n\
    device                     path of device, default value is: %s\n\
    rgb_color[B:G:R]           default value is: 0x%06x\n", DEFAULT_DEVICE, DEFAULT_COLOR);
}

#ifdef CMD_ALL_IN_ONE
int cmd_clear(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0;
	const char *dev_name = DEFAULT_DEVICE;
	_lcd_st7735s_prop_t prop;
	_lcd_st7735s_clear_t data;
	uint32_t color = DEFAULT_COLOR;
	uint16_t c;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-D") == 0) {
			dev_name = argv[++n];
		} else {
			break;
		}
	}
	argc -= (--n);

	if (!(argc <= 2 || argc == 5 || argc == 6)) {
		show_usage();
		exit(1);
	}
	if (argc == 2) {
		if (!(*argv[n + 1] == '0' || *argv[n + 1] == '1')) {
			show_usage();
			exit(1);
		}
	}

	if ((f = open(dev_name, O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	memset(&data, 0, sizeof(data));
	if (argc <= 2) {
		if ((err = ioctl(f, LCD_ST7735S_IOC_GET_PROP, &prop)) != 0) {
			perror("Ioctl");
			close(f);
			return err;
		}

		data.x = data.y = 0;
		data.w = prop.swap_xy ? prop.h : prop.w;
		data.h = prop.swap_xy ? prop.w : prop.h;
		if (argc == 2)
			color = (uint32_t)strtoul(argv[n + 1], NULL, 16);
	} else {
		data.x = (uint8_t)atoi(argv[n + 1]);
		data.y = (uint8_t)atoi(argv[n + 2]);
		data.w = (uint8_t)atoi(argv[n + 3]);
		data.h = (uint8_t)atoi(argv[n + 4]);
		if (argc == 6)
			color = (uint32_t)strtoul(argv[n + 5], NULL, 16);
	}

	c = (color & 0x000000f8) << 8; /* red */
	c |= (color & 0x0000fc00) >> 5; /* green */
	c |= (color & 0x00f80000) >> 19; /* blue */

	if ((err = ioctl(f, LCD_ST7735S_IOC_SET_LOW_COLOR, &c)) != 0)
		perror("Ioctl");
	else if ((err = ioctl(f, LCD_ST7735S_IOC_CLEAR, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

