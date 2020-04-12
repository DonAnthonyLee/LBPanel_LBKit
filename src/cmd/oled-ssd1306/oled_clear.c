/* --------------------------------------------------------------------------
 *
 * Commands for OLED SSD1306
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
 * File: oled_clear.c
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
	printf("oled_clear - Clear OLED\n\n");
	printf("Usage: oled_clear [-v] [-D device] x y width height [erase_mode]\n\
    device                     path of device, default value is: %s\n\
    erase_mode = 0,1           default value is: 1\n", DEFAULT_DEVICE);
}

#ifdef CMD_ALL_IN_ONE
int cmd_clear(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0, show_msg = 0;
	const char *dev_name = DEFAULT_DEVICE;
	_oled_ssd1306_clear_t data;
	_oled_ssd1306_prop_t prop;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-D") == 0) {
			dev_name = argv[++n];
		} else if (strcmp(argv[n], "-v") == 0) {
			show_msg = 1;
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

	memset(&prop, 0, sizeof(prop));
	if ((err = ioctl(f, OLED_SSD1306_IOC_GET_PROP, &prop)) != 0) {
		prop.w = 128;
		prop.h = 64;
	}
	if (show_msg) {
		fprintf(stdout, "Panel's width: %u\nPanel's height: %u\n", prop.w, prop.h);
	}

	memset(&data, 0, sizeof(data));
	if (argc <= 2) {
		data.x = data.y = 0;
		data.w = prop.w;
		data.h = prop.h;
		if(!(argc == 1 || *argv[n + 1] == '1')) memset(data.patterns, (int)0xff, sizeof(data.patterns));
	} else {
		data.x = (uint8_t)atoi(argv[n + 1]);
		data.y = (uint8_t)atoi(argv[n + 2]);
		data.w = (uint8_t)atoi(argv[n + 3]);
		data.h = (uint8_t)atoi(argv[n + 4]);
		if(!(argc < 6 || *argv[n + 5] == '1')) memset(data.patterns, (int)0xff, sizeof(data.patterns));
	}

	if ((err = ioctl(f, OLED_SSD1306_IOC_CLEAR, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

