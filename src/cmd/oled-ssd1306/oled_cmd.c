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
 * File: oled_cmd.c
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

extern int cmd_power(int argc, char **argv);
extern int cmd_clear(int argc, char **argv);
extern int cmd_show(int argc, char **argv);
extern int cmd_update(int argc, char **argv);

static char org_cmd[] = "oled_cmd";

static int show_usage(int argc, char **argv)
{
	printf("%s - Commands for OLED SSD1306\n\n", org_cmd);
	printf("Usage: %s [commad] cmd_options\n\
    Valid commands:\n\
        power                   turn screen's power on/off\n\
        clear                   clear screen\n\
        show                    show characters on screen\n\
        update                  whether to update screen when drawing\n", org_cmd);

	return -1;
}

int main(int argc, char **argv)
{
	int (*func)(int, char**) = NULL;
	int offset = 0;

	char cmd[128];
	char *str;

	memset(cmd, 0, sizeof(cmd));

	str = strrchr(argv[0], '/');
	strncpy(cmd, (str == NULL ? argv[0] : str + 1), sizeof(cmd) - 1);

	if (strcmp(cmd, org_cmd) == 0 && argc > 1) {
		strncpy(cmd, "oled_", 5);
		strncpy(&cmd[5], argv[1], sizeof(cmd) - 6);
		offset = 1;
	}

	if (strncmp(cmd, "oled_", 5) == 0) {
		if (strcmp(&cmd[5], "power") == 0)
			func = cmd_power;
		else if (strcmp(&cmd[5], "clear") == 0)
			func = cmd_clear;
		else if (strcmp(&cmd[5], "show") == 0)
			func = cmd_show;
		else if (strcmp(&cmd[5], "update") == 0)
			func = cmd_update;
	}

	if (func == NULL)
		func = show_usage;

	return (*func)(argc - offset, argv + offset);
}

