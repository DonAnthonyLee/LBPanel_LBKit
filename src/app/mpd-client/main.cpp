/* --------------------------------------------------------------------------
 *
 * Simple MPD client using LBKit
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

#include <stdio.h>
#include <lbk/LBKit.h>

#include "TApplication.h"


void show_usage(const char *cmd)
{
	printf("Usage: %s [options]\n", cmd);
	printf("Options:\n\
    --conf config_path         Use the specified file as config file\n");
}


int main(int argc, char **argv)
{
	BPath path_conf("/etc/LBMPClient.conf");
	BFile f;

	for(int n = 1; n < argc; n++)
	{
		if (n < argc - 1 && strcmp(argv[n], "--conf") == 0)
		{
			path_conf.SetTo(argv[++n], NULL, true);
		}
		else
		{
			show_usage(argv[0]);
			exit(-1);
		}
	}

	BString strConfSettings;

	LBAppSettings cfg;
	if(f.SetTo(path_conf.Path(), B_READ_ONLY) != B_OK || cfg.AddItems(&f) == false)
		fprintf(stderr, "Unable to open config file (%s) !\n", path_conf.Path());
	else
		strConfSettings << "LBMPClient::Config=" << path_conf.Path();
	f.Unset();

	if(strConfSettings.Length() > 0)
		cfg.AddItem(strConfSettings.String(), 0);
	cfg.AddItem("IPC=LBMPClient", 0);

	TApplication *app = new TApplication(&cfg);
	cfg.MakeEmpty();

	app->Go();

	app->Lock();
	app->Quit();

	return 0;
}

