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

#include "TApplication.h"
#include "TCommandsPageView.h"

#define MSG_SYSTEM_MENU		'msys'
#define MSG_CUSTOM_MENU		'mcus'
#define MSG_SYSTEM_MENU_ITEM	'msyi'
#define MSG_CUSTOM_MENU_ITEM	'mcui'
#define MSG_MODULE_ITEM		'modi'


static const t_menu_item system_menus[] = {
	{(char*)"返回", NULL, NULL},
	{(char*)"OTG 设备功能设置", (char*)"/usr/share/scripts/usb-gadget.sh", NULL},
	{(char*)"覆盖文件分区切换", (char*)"/usr/share/scripts/overlay-switch.sh", NULL},
};


TListItem::TListItem(const char *text, BMessage *msg)
	: LBListStringItem(text), fMessage(msg)
{
}


TListItem::~TListItem()
{
	if(fMessage) delete fMessage;
}


const BMessage*
TListItem::Message() const
{
	return fMessage;
}


TListView::TListView(const char *name)
	: LBListView(3, name)
{
}


status_t
TListView::Invoke(const BMessage *msg)
{
	do
	{
		if(msg != NULL || Message() == NULL) break;

		TListItem *item = e_cast_as(CurrentSelection(), TListItem);
		if(item == NULL || item->Message() == NULL) break;

		const char *cmd = NULL;
		const char *args = NULL;
		if(item->Message()->FindString("command", &cmd) != B_OK || cmd == NULL) break;
		item->Message()->FindString("args", &args);

		BMessage aMsg(*(Message()));
		aMsg.AddString("command", cmd);
		if(args != NULL)
			aMsg.AddString("args", args);

		return LBListView::Invoke(&aMsg);
	} while(false);

	return LBListView::Invoke(msg);
}


void
TListView::KeyUp(uint8 key, uint8 clicks)
{
	LBListView::KeyUp(key, clicks);

	if(MasterView()->StandingInView() == this &&
	   clicks > 1 && clicks != 0xff &&
	   key == 1) // canceled by user
	{
		StandBack();
	}
}


void
TListView::FlexibleKeyDown(uint16 key, uint8 clicks)
{
	switch(key)
	{
		case B_ESCAPE:
			StandBack();
			break;

		default:
			LBListView::FlexibleKeyDown(key, clicks);
	}
}


TCommandsPageView::TCommandsPageView(const char *name)
	: LBMenuView(name),
	  fBlockKeyEventsTimestamp(0)
{
	AddItem(new LBMenuItem("系统功能", new BMessage(MSG_SYSTEM_MENU), LBK_ICON_SYSTEM));
	AddItem(new LBMenuItem("自定功能", new BMessage(MSG_CUSTOM_MENU), LBK_ICON_CUSTOM));

	LBListView *listView = new TListView("system");
	for(int k = 0; k < (int)(sizeof(system_menus) / sizeof(system_menus[0])); k++)
	{
		listView->AddItem(new LBListStringItem(system_menus[k].title));
	}
	listView->SetMessage(new BMessage(MSG_SYSTEM_MENU_ITEM));
	listView->MakeSelectable(true);
	listView->SetAutoStandBack(false);
	AddStickView(listView);
}


TCommandsPageView::~TCommandsPageView()
{
	EmptyModuleItems();
}


int32
TCommandsPageView::VisibleItemsCountMax() const
{
	return 2;
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
TCommandsPageView::FlexibleKeyDown(uint16 key, uint8 clicks)
{
	switch(key)
	{
		case B_PAGE_UP:
			SwitchToPrevPage();
			break;

		case B_PAGE_DOWN:
			SwitchToNextPage();
			break;

		default:
			LBMenuView::FlexibleKeyDown(key, clicks);
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


status_t
TCommandsPageView::ExecCommand(const char *command, const char *args)
{
	BEntry entry(command);

	if(IsActivated() == false)
	{
		// should never happened here, just in case.
		return B_ERROR;
	}
	else if(entry.Exists() == false)
	{
		LBView *alertView = new LBAlertView("错误", "文件不存在!",
						    LBK_ICON_NONE, LBK_ICON_OK, LBK_ICON_NONE,
						    B_WARNING_ALERT);

		// LBAlertView will run StandBack() automatically when it has no invoker.
		AddStickView(alertView);
		alertView->StandIn();
	}
	else
	{
		BString cmdStr(command);
		if(args != NULL)
			cmdStr << " " << args;

		LBPanelDevice *dev = PanelDevice();

		dev->BlockKeyEvents(true);
		int err = system(cmdStr.String());
		fBlockKeyEventsTimestamp = system_time();
		dev->BlockKeyEvents(false);

		if(err < 0 || err == 127)
		{
			LBView *alertView = new LBAlertView("错误", "无法执行命令!",
							     LBK_ICON_NONE, LBK_ICON_OK, LBK_ICON_NONE,
							     B_WARNING_ALERT);

			// LBAlertView will run StandBack() automatically when it has no invoker.
			AddStickView(alertView);
			alertView->StandIn();
		}
		else
		{
			// NOTE: Things have been changed since we changed the path of updating
			LBView *currView = this;
			while(currView->StandingInView() != NULL)
				currView = currView->StandingInView();
			currView->Invalidate();

			return B_OK;
		}
	}

	return B_ERROR;
}


void
TCommandsPageView::AddModuleItem(LBView *view, const char *title, const lbk_icon *icon)
{
	if(view == NULL || view->MasterView() != NULL || AddStickView(view) == false) return;

	int32 n = CountItems();
	BMessage *msg = new BMessage(MSG_MODULE_ITEM);
	msg->AddPointer("view", reinterpret_cast<void*>(view));

#if 0
	// TODO: implement LBMenuItem to support lbk_icon more than lbk_icon_id
	AddItem(new LBMenuItem(title, msg, icon), n - 1);
#else
	// Note: Just for test
	AddItem(new LBMenuItem(title, msg, LBK_ICON_NONE), n - 1);
#endif
}


void
TCommandsPageView::EmptyModuleItems()
{
	while(CountItems() > 2)
	{
		LBMenuItem *item = e_cast_as(ItemAt(1), LBMenuItem);
		BMessage *msg = item->Message();

		void *source = NULL;
		if(msg->FindPointer("view", &source) == B_OK && source != NULL)
		{
			// remove stick view without deleting
			RemoveStickView(reinterpret_cast<LBView*>(source));
		}

		RemoveItem(item);
		delete item;
	}
}


void
TCommandsPageView::MessageReceived(BMessage *msg)
{
	bigtime_t when;
	int32 index;
	LBView *view;
	const char *command;

	switch(msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
			if(msg->FindInt64("when", &when) != B_OK) break;
			if(fBlockKeyEventsTimestamp > when) break;
			LBMenuView::MessageReceived(msg);
			break;

		case MSG_SYSTEM_MENU:
			if(IsStoodIn() || (view = FindStickView("system")) == NULL) break;
			view->StandIn();
			break;

		case MSG_SYSTEM_MENU_ITEM:
			if((view = FindStickView("system")) == NULL || StandingInView() != view) break;
			if(msg->FindInt32("index", &index) != B_OK) break;
			if(index < 0 || index >= (int32)(sizeof(system_menus) / sizeof(system_menus[0]))) break;
			if(system_menus[index].command != NULL)
			{
				if(ExecCommand(system_menus[index].command, system_menus[index].args) == B_OK) break;
			}
			view->StandBack();
			break;

		case MSG_CUSTOM_MENU:
			{
				TApplication *app = e_cast_as(Looper(), TApplication);
				if(app == NULL) break;

				TListView *listView = new TListView("custom");
				listView->AddItem(new TListItem("返回", NULL));
				for(int32 k = 0; k < app->CountCustomMenuItems(); k++)
				{
					const t_menu_item *item = app->CustomMenuItemAt(k);
					if(item->title == NULL || item->command == NULL) continue;

					BMessage *aMsg = new BMessage();
					aMsg->AddString("command", item->command);
					if(item->args != NULL)
						aMsg->AddString("args", item->args);

					listView->AddItem(new TListItem(item->title, aMsg));
				}
				listView->SetMessage(new BMessage(MSG_CUSTOM_MENU_ITEM));
				listView->MakeSelectable(true);
				listView->SetAutoStandBack(false);

				AddStickView(listView);
				listView->SetTarget(this);
				listView->StandIn();
			}
			break;

		case MSG_CUSTOM_MENU_ITEM:
			if((view = FindStickView("custom")) == NULL || StandingInView() != view) break;

			if(msg->FindString("command", &command) == B_OK)
			{
				const char *args = NULL;
				msg->FindString("args", &args);

				if(ExecCommand(command, args) == B_OK) break;
			}

			view->StandBack();
			break;

		case MSG_MODULE_ITEM:
			{
				void *source = NULL;
				if(msg->FindPointer("view", &source) != B_OK || source == NULL) break;
				
				LBView *view = reinterpret_cast<LBView*>(source);
				view->SetName("module"); // see LBK_VIEW_STOOD_BACK
				view->StandIn();
			}
			break;

		case LBK_VIEW_STOOD_BACK:
			{
				void *source = NULL;
				if(msg->FindPointer("view", &source) != B_OK || source == NULL) break;

				LBView *stickView = reinterpret_cast<LBView*>(source);
				if(stickView->Name() == NULL ||
				   !(strcmp(stickView->Name(), "system") == 0 ||
				     strcmp(stickView->Name(), "module") == 0))
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
