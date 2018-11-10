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
 * File: TCommandsPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include "TCommandsPageView.h"

#define MSG_SYSTEM_MENU		'msys'
#define MSG_CUSTOM_MENU		'mcus'


TCommandsPageView::TCommandsPageView(const char *name)
	: LBMenuView(name)
{
	AddItem(new LBMenuItem("系统功能", new BMessage(MSG_SYSTEM_MENU), LBK_ICON_SYSTEM));
	AddItem(new LBMenuItem("自定功能", new BMessage(MSG_CUSTOM_MENU), LBK_ICON_CUSTOM));
}


TCommandsPageView::~TCommandsPageView()
{
	// TODO
}


void
TCommandsPageView::KeyUp(uint8 key, uint8 clicks)
{
	LBMenuView::KeyUp(key, clicks);

	if(clicks > 1 && clicks != 0xff)
	{
		if(key == 0) // Left
			SwitchToPrevPage();
		else if(key == 2) // Right
			SwitchToNextPage();
		return;
	}
}


void
TCommandsPageView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case MSG_SYSTEM_MENU:
			// TODO
			break;

		case MSG_CUSTOM_MENU:
			// TODO
			break;

		default:
			LBMenuView::MessageReceived(msg);
	}
}

