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
 * File: main.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <lbk/LBKit.h>
#include "TMainPageView.h"
#include "TMenuPageView.h"

#ifdef ETK_MAJOR_VERSION
	#undef SEEK_END
	#undef SEEK_CUR
	#undef SEEK_SET

	#define SEEK_SET	E_SEEK_SET
	#define SEEK_CUR	E_SEEK_CUR
	#define SEEK_END	E_SEEK_END
#endif


void show_usage(const char *cmd)
{
	printf("Usage: %s [options]\n", cmd);
	printf("Options:\n");
	printf("\t--conf config_path,		use the specified file as config file\n");
}


int main(int argc, char **argv)
{
	BPath path_conf("/etc/LBPanel.conf");
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
			exit(-1);
		}
	}

	BList cfg;
	if(f.SetTo(path_conf.Path(), B_READ_ONLY) != B_OK)
	{
		fprintf(stderr, "Unable to open config file (%s), use default settings !\n", path_conf.Path());

#if 1
		cfg.AddItem(new BString("PanelDeviceAddon=/usr/lib/add-ons/lbk/npi-oled-hat.so"));
#else
		cfg.AddItem(new BString("PanelDeviceAddon=/usr/lib/add-ons/lbk/vpd.so,point_size=2,width=128,height=64"));
#endif
		cfg.AddItem(new BString("IPC=LBPanel"));

		// TODO: more
	}
	else
	{
		char *buf = NULL;
		off_t fsize = (off_t)f.Seek(0, SEEK_END);
		f.Seek(0, SEEK_SET);

		if(fsize == (off_t)-1 || fsize > 65535 ||
		   (buf = (char*)malloc((size_t)fsize)) == NULL ||
		   f.Read(buf, fsize) != (ssize_t)fsize)
		{
			if(buf != NULL) free(buf);
			fprintf(stderr, "Unable to read config file (%s) !\n", path_conf.Path());
			exit(-1);
		}

		BString str(buf);
		free(buf);

		int32 offset = 0, found;
		while((found = str.FindFirst("\n", offset)) > 0)
		{
			if(found > offset && str[offset] != '#')
			{
				BString *item = new BString(str.String() + offset, found - offset);
				cfg.AddItem(item);
			}
			offset = found + 1;
		}

		cfg.AddItem(new BString("IPC=LBPanel"), 0);
	}
	f.Unset();

	LBApplication *app = new LBApplication(&cfg);
	for(int32 k = 0; k < cfg.CountItems(); k++) delete ((BString*)cfg.ItemAt(k));

	// TODO: Screen timeout, tools, custom commands, etc. IT'S A LOT ...
	app->AddPageView(new TMainPageView(), false);
	app->AddPageView(new TMenuPageView(), false);

	app->Go();

	app->Lock();
	app->Quit();

	return 0;
}

