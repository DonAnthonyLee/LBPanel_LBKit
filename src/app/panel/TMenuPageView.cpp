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
#include "TAboutView.h"

#define MSG_TURN_OFF_SCREEN	'mtos'
#define MSG_REBOOT		'mreb'
#define MSG_POWER_OFF		'mpwr'
#define MSG_ABOUT		'mabo'
#define MSG_EXIT		'mext'

#define MSG_POWER_CONFIRM_MSG	'mpwc'
#define MSG_REBOOT_GO		'mrGO'
#define MSG_POWER_OFF_GO	'mpOF'


TMenuPageView::TMenuPageView(const char *name)
	: LBMenuView(name)
{
	AddItem(new LBMenuItem("关闭屏幕", new BMessage(MSG_TURN_OFF_SCREEN), LBK_ICON_SCREEN));
	AddItem(new LBMenuItem("重新启动", new BMessage(MSG_REBOOT), LBK_ICON_REBOOT));
	AddItem(new LBMenuItem("关闭机器", new BMessage(MSG_POWER_OFF), LBK_ICON_POWER_OFF));
	AddItem(new LBMenuItem("简介", new BMessage(MSG_ABOUT), LBK_ICON_ABOUT));
	AddItem(new LBMenuItem("退出", new BMessage(MSG_EXIT), LBK_ICON_EXIT));
}


TMenuPageView::~TMenuPageView()
{
}


void
TMenuPageView::KeyUp(uint8 key, uint8 clicks)
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
TMenuPageView::MessageReceived(BMessage *msg)
{
	int32 which = -1;
	uint8 clicks = 0;
	LBAlertView *view;
	bool isPowerOff;

	switch(msg->what)
	{
		case MSG_TURN_OFF_SCREEN:
			printf("[TMenuPageView]: Turning off screen.\n");
			SetPowerState(false);
			break;

		case MSG_REBOOT:
		case MSG_POWER_OFF:
			view = new LBAlertView((msg->what == MSG_REBOOT) ? "重新启动" : "关闭机器",
					       (msg->what == MSG_REBOOT) ? "是否确定\n进行重启操作?" : "是否确定\n进行关机操作?",
					       LBK_ICON_NO, LBK_ICON_YES, LBK_ICON_NONE,
					       B_WARNING_ALERT);
			view->SetInvoker(new BInvoker(new BMessage(MSG_POWER_CONFIRM_MSG), this));
			view->SetName((msg->what == MSG_REBOOT) ? "RebootRequested" : "PowerOffRequested");
			AddStickView(view);
			view->StandIn();
			printf("[TMainPageView]: %s requested.\n", (msg->what == MSG_REBOOT) ? "Reboot" : "Power off");
			break;

		case MSG_POWER_CONFIRM_MSG:
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(msg->FindInt32("which", &which) != B_OK) break;
			if(clicks > 1) break;

			view = e_cast_as(StickViewAt(0), LBAlertView);
			if(view != StandingInView()) break;
			isPowerOff = (BString("PowerOffRequested") == view->Name());

			// the view will be deleted in handling of "LBK_VIEW_STOOD_BACK"
			view->StandBack();

			if(which != 1) break;
			view = new LBAlertView(isPowerOff ? "关闭机器" : "重新启动",
					       isPowerOff ? "正在关机，请稍候..." : "正在重启，请稍候...",
					       LBK_ICON_NONE, LBK_ICON_NONE, LBK_ICON_NONE,
					       B_EMPTY_ALERT);
			AddStickView(view);
			view->StandIn();

			Looper()->PostMessage(isPowerOff ? MSG_POWER_OFF_GO : MSG_REBOOT_GO, this);
			break;

		case MSG_REBOOT_GO:
		case MSG_POWER_OFF_GO:
			if(msg->HasBool("delay") == false) // in order to show the "Shutting down..."
			{
				msg->AddBool("delay", true);
				Looper()->PostMessage(msg, this);
				break;
			}

			printf("[TMainPageView]: Going to %s !\n", (msg->what == MSG_REBOOT_GO) ? "reboot" : "power off");
			if(system((msg->what == MSG_REBOOT_GO) ? "reboot" : "poweroff") < 0)
			{
				view = new LBAlertView("错误",
						       (msg->what == MSG_REBOOT_GO) ? "无法重启!" : "无法关机!",
						       LBK_ICON_NONE, LBK_ICON_OK, LBK_ICON_NONE,
						       B_STOP_ALERT);
				AddStickView(view);
				view->StandIn();
			}
			break;

		case MSG_ABOUT:
			AddStickView(new TAboutView("About"));
			break;

		case LBK_VIEW_STOOD_BACK:
			{
				void *source = NULL;
				if(msg->FindPointer("view", &source) != B_OK || source == NULL) break;

				LBView *stickView = reinterpret_cast<LBView*>(source);
				if(stickView)
				{
					RemoveStickView(stickView);
					delete stickView;
				}
			}
			break;

		case MSG_EXIT:
			printf("[TMenuPageView]: Exiting...\n");
			SetPowerState(false); // power off the screen
			Looper()->PostMessage(LBK_QUIT_REQUESTED);
			break;

		default:
			LBMenuView::MessageReceived(msg);
	}
}

