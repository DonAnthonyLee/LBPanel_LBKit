/* --------------------------------------------------------------------------
 *
 * Panel application for little board
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
 * File: lbk-cmd.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

extern int cmd_notify(int argc, char **argv);
extern int cmd_menu(int argc, char **argv);
extern int cmd_message(int argc, char **argv);

static char org_cmd[] = "lbk-cmd";

static int show_usage(int argc, char **argv)
{
	printf("%s - Utilities of LBKit\n\n", org_cmd);
	printf("Usage: %s [commad] cmd_options\n\
    Valid commands:\n\
        notify                  Notify the application using LBKit\n\
        message                 Display message on specified panel device\n\
        menu                    Display menu on specified panel device\n", org_cmd);

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
		strncpy(cmd, "lbk-", 4);
		strncpy(&cmd[4], argv[1], sizeof(cmd) - 5);
		offset = 1;
	}

	if (strncmp(cmd, "lbk-", 4) == 0) {
		if (strcmp(&cmd[4], "notify") == 0)
			func = cmd_notify;
		else if (strcmp(&cmd[4], "menu") == 0)
			func = cmd_menu;
		else if (strcmp(&cmd[4], "message") == 0)
			func = cmd_message;
	}

	if (func == NULL)
		func = show_usage;

	return (*func)(argc - offset, argv + offset);
}

