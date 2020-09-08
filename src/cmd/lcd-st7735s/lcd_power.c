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
 * File: lcd_power.c
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

static void show_usage(void)
{
	char *dev_name = lcd_st7735s_probe_default_device();

	printf("lcd_power - Turn LCD's power on/off\n\n\n");
	printf("Usage: lcd_power [-D device] [state]\n\
    device                path of device, default value is: %s\n\
    state = 0,1           default value is 1\n", (dev_name ? dev_name : "none"));

	if (dev_name != NULL)
		free(dev_name);
}

#ifdef CMD_ALL_IN_ONE
int cmd_power(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int n, f, err = 0;
	char *dev_name = NULL;
	_lcd_st7735s_power_t data;

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

	if (argc > 2) {
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

	data.state = (argc == 1 || *(argv[n + 1]) == '1') ? 1 : 0;

	if ((err = ioctl(f, LCD_ST7735S_IOC_POWER, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

