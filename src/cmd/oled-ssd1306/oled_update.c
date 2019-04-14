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
 * File: oled_update.c
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
	printf("oled_update - Whether to update OLED when drawing\n\n");
	printf("Usage: oled_update [-D device] [state]\n\
    device                path of device, default value is: %s\n\
    state = 0,1           default value is: 1\n", DEFAULT_DEVICE);
}

#ifdef CMD_ALL_IN_ONE
int cmd_update(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0;
	const char *dev_name = DEFAULT_DEVICE;
	bool state;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-D") == 0) {
			dev_name = argv[++n];
		} else {
			break;
		}
	}
	argc -= (--n);

	if (argc > 2) {
		show_usage();
		exit(1);
	}

	if ((f = open(dev_name, O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	state = (argc == 1 || *(argv[n + 1]) == '1') ? true : false;

	if ((err = ioctl(f, OLED_SSD1306_IOC_UPDATE, &state)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

