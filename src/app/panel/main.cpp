/* --------------------------------------------------------------------------
 *
 * Panel Application for NanoPi OLED Hat
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
 * File: main.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <OLEDApp.h>
#include "TMainPageView.h"


void show_usage(const char *cmd)
{
	printf("Usage: %s [options]\n", cmd);
	printf("Options:\n");
	printf("\t--conf config_path,		use the specified file as config file\n");
}


int main(int argc, char **argv)
{
	BPath path_conf("/etc/oled_panel.conf");
	BPath path_oled_dev("/dev/oled-003c");
	BPath path_input_dev("/dev/input/event0");
	BFile f;

	for(int n = 1; n < argc; n++)
	{
		if (n < argc - 1 && strcmp(argv[n], "--conf") == 0)
		{
			path_conf.SetTo(argv[++n]);
		}
		else
		{
			show_usage(argv[0]);
			exit(1);
		}
	}

#if 0
	if (f.SetTo(path_conf.Path(), B_READ_ONLY) != B_OK)
	{
		printf("Unable to open config file (%s) !\n", path_conf.Path());
		exit(1);
	}

	// TODO: parsing the config file

	f.Unset();
#endif

	int fd1, fd2;
	if ((fd1 = open(path_oled_dev.Path(), O_RDWR)) < 0)
	{
		perror("Unable to open OLED device");
		exit(1);
	}

	if ((fd2 = open(path_input_dev.Path(), O_RDONLY)) < 0)
	{
		perror("Unable to open input device");
		exit(1);
	}


	OLEDApp app(fd1, fd2);

	// TODO: a lot
	app.AddPageView(new TMainPageView(), false);
	app.Go(); // the "app" only handle input events

	close(fd1);
	close(fd2);

	printf("End\n");
	return 0;
}

