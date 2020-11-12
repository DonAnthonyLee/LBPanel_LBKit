/* --------------------------------------------------------------------------
 *
 * Simple MPD Client using LBKit
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
 * File: TApplication.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>

#include "TApplication.h"
#include "TMainPageView.h"


TApplication::TApplication(const LBAppSettings *settings)
	: LBApplication(settings)
{
	if(CountPanels() == 0) return;

	// TODO
	AddPageView(new TMainPageView(), false);

	LoadConfig(settings);
}


TApplication::~TApplication()
{
	// TODO
}


void
TApplication::LoadConfig(const LBAppSettings *cfg)
{
	// TODO
}


void
TApplication::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		// TODO
		case LBK_APP_SETTINGS_UPDATED:
			if(fConfigPath.Path() != NULL)
			{
				LBAppSettings cfg;
				BFile f(fConfigPath.Path(), B_READ_ONLY);

				if(f.InitCheck() != B_OK || cfg.AddItems(&f) == false) break;
				LoadConfig(&cfg);

				// Just for debug
				fprintf(stdout, "[TApplication]: Settings updated.\n");
			}
			break;

		default:
			LBApplication::MessageReceived(msg);
	}
};

