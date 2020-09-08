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
 * File: lcd_probe.c
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_DEVICE_PREFIX	"/dev/spi-lcd"
#define SPI_MAX_BUS_NUMS	2
#define SPI_MAX_CS_NUMS		3

char* lcd_st7735s_probe_default_device(void)
{
	char dev_name[256];
	int i, j, f;

	for (i = 0; i < SPI_MAX_BUS_NUMS; i++) {
		for (j = 0; j < SPI_MAX_CS_NUMS; j++) {
			memset(dev_name, 0, sizeof(dev_name));
			snprintf(dev_name, sizeof(dev_name), "%s%d.%d", DEFAULT_DEVICE_PREFIX, i, j);
			if ((f = open(dev_name, O_RDWR)) < 0) continue;
			close(f);
			return strdup(dev_name);
		}
	}

	return NULL;
}

