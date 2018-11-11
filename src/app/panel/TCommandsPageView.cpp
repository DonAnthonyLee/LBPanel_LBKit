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
#define MSG_SYSTEM_MENU_ITEM	'msyi'
#define MSG_CUSTOM_MENU_ITEM	'mcui'

struct tMenuItem {
	const char *title;
	const char *command;
	const char *args;
};


static struct tMenuItem system_menus[] = {
	{"OTG 设备功能设置", "/usr/share/scripts/usb-gadget.sh", NULL},
	{"覆盖文件分区切换", "/usr/share/scripts/overlay-switch.sh", NULL},
};


TCommandsPageView::TCommandsPageView(const char *name)
	: LBMenuView(name)
{
	AddItem(new LBMenuItem("系统功能", new BMessage(MSG_SYSTEM_MENU), LBK_ICON_SYSTEM));
	AddItem(new LBMenuItem("自定功能", new BMessage(MSG_CUSTOM_MENU), LBK_ICON_CUSTOM));


	LBListView *listView = new LBListView(3, "system");
	for(int k = 0; k < (int)(sizeof(system_menus) / sizeof(system_menus[0])); k++)
	{
		listView->AddItem(new LBListStringItem(system_menus[k].title));
	}
	listView->SetMessage(new BMessage(MSG_SYSTEM_MENU_ITEM));
	listView->MakeSelectable(true);
	AddStickView(listView);
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
TCommandsPageView::Attached()
{
	LBMenuView::Attached();

	LBListView *view = e_cast_as(FindStickView("system"), LBListView);
	if(view != NULL)
	{
		view->SetPosition(0);
		view->SetTarget(this);
	}
}


void
TCommandsPageView::MessageReceived(BMessage *msg)
{
	int32 index;
	LBView *view;

	switch(msg->what)
	{
		case MSG_SYSTEM_MENU:
			if(IsStoodIn() || (view = FindStickView("system")) == NULL) break;
			view->StandIn();
			break;

		case MSG_SYSTEM_MENU_ITEM:
			if(msg->FindInt32("index", &index) != B_OK) break;
			if(index < 0 || index >= (int32)(sizeof(system_menus) / sizeof(system_menus[0]))) break;
			if(system_menus[index].command != NULL)
			{
				BEntry entry(system_menus[index].command);

				if(entry.Exists() == false)
				{
					view = new LBAlertView("错误", "文件不存在!",
							       LBK_ICON_NONE, LBK_ICON_OK, LBK_ICON_NONE,
							       B_WARNING_ALERT);

					// LBAlertView will run StandBack() automatically when it has no invoker.
					AddStickView(view);
					view->StandIn();
					break;
				}
				else
				{
					BString cmdStr(system_menus[index].command);
					if(system_menus[index].args != NULL)
						cmdStr << " " << system_menus[index].args;

					int err = system(cmdStr.String());
					if(err < 0 || err == 127)
					{
						view = new LBAlertView("错误", "无法执行命令!",
								       LBK_ICON_NONE, LBK_ICON_OK, LBK_ICON_NONE,
								       B_WARNING_ALERT);

						// LBAlertView will run StandBack() automatically when it has no invoker.
						AddStickView(view);
						view->StandIn();
						break;
					}
				}

				Invalidate();
			}
			break;


		case MSG_CUSTOM_MENU:
			// TODO
			break;

		case MSG_CUSTOM_MENU_ITEM:
			// TODO
			break;

		case LBK_VIEW_STOOD_BACK:
			{
				void *source = NULL;
				if(msg->FindPointer("view", &source) != B_OK || source == NULL) break;

				LBView *stickView = reinterpret_cast<LBView*>(source);
				if(stickView->Name() == NULL ||
				   !(strcmp(stickView->Name(), "system") == 0))
				{
					RemoveStickView(stickView);
					delete stickView;
				}
			}
			break;

		default:
			LBMenuView::MessageReceived(msg);
	}
}


