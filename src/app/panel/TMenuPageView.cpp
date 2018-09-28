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
 * File: TMenuPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include "TMenuPageView.h"

#define MSG_TURN_OFF_SCREEN	'mtos'
#define MSG_REBOOT		'mreb'
#define MSG_POWER_OFF		'mpwr'


TMenuPageView::TMenuPageView(const char *name)
	: LBMenuView(name)
{
	AddItem(new LBMenuItem("关闭屏幕", new BMessage(MSG_TURN_OFF_SCREEN), LBK_ICON_SCREEN));
	AddItem(new LBMenuItem("重新启动", new BMessage(MSG_REBOOT), LBK_ICON_REBOOT));
	AddItem(new LBMenuItem("关闭机器", new BMessage(MSG_POWER_OFF), LBK_ICON_POWER_OFF));
}


TMenuPageView::~TMenuPageView()
{
	// TODO
}


void
TMenuPageView::KeyUp(uint8 key, uint8 clicks)
{
	if(clicks > 1 && clicks != 0xff)
	{
		if(key == 0) // Left
			SwitchToPrevPage();
		else if(key == 2) // Right
			SwitchToNextPage();
		return;
	}

	LBMenuView::KeyUp(key, clicks);
}


void
TMenuPageView::Attached()
{
	for(int32 k = 0; k < CountItems(); k++)
		ItemAt(k)->SetTarget(this);
}


void
TMenuPageView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case MSG_TURN_OFF_SCREEN:
			printf("[TMenuPageView]: Turning off screen.\n");
			SetPowerState(false);
			break;

		case MSG_REBOOT:
			// TODO
			printf("[TMenuPageView]: Reboot requested.\n");
			break;

		case MSG_POWER_OFF:
			printf("[TMenuPageView]: Power off requested.\n");
			// TODO
			break;

		default:
			LBMenuView::MessageReceived(msg);
	}
}

