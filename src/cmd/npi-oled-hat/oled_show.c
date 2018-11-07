/* --------------------------------------------------------------------------
 *
 * Commands for NanoPi OLED Hat
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
 * File: oled_show.c
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

#include <oled_ssd1306_ioctl.h>

#define DEFAULT_DEVICE		"/dev/oled-003c"

static void show_usage(void)
{
	printf("oled_show - Show characters on OLED\n\n");
	printf("Usage: oled_show [-D device] x y size string [erase_mode]\n\
    device                     path of device, default value is: %s\n\
    size = 8,12,14,16,24,32,0    0 means 32x32 icon, range: 0~9.\n\
    erase_mode = 0,1           default value is: 0\n", DEFAULT_DEVICE);
}

#ifdef CMD_ALL_IN_ONE
int cmd_show(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0;
	const char *dev_name = DEFAULT_DEVICE;
	_oled_ssd1306_show_t data;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-D") == 0) {
			dev_name = argv[++n];
		} else {
			break;
		}
	}
	argc -= (--n);

	if (argc < 5 || argc > 6) {
		show_usage();
		exit(1);
	}

	if ((f = open(dev_name, O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	data.x = (int16_t)atoi(argv[n + 1]);
	data.y = (int16_t)atoi(argv[n + 2]);
	data.size = (uint8_t)atoi(argv[n + 3]);
	strncpy(data.str, argv[n + 4], sizeof(data.str));
	data.erase_mode = (argc < 6 || *argv[n + 5] != '1') ? false : true;

	if ((err = ioctl(f, OLED_SSD1306_IOC_SHOW, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

