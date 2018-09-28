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
 * File: TMainPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <time.h>

#include "TMainPageView.h"


#define POWER_REQUESTED_CONFIRM_MSG	'pwof'


TMainPageView::TMainPageView(const char *name)
	: LBPageView(name), fTabIndex(0), f24Hours(true), fShowSeconds(false), fShowTimestamp(0)
{
	SetNavButtonIcon(0, LBK_ICON_LEFT);
	SetNavButtonIcon(2, LBK_ICON_RIGHT);

	// TODO
}

TMainPageView::~TMainPageView()
{
}


void
TMainPageView::Pulse()
{
	if(fTabIndex != 0) return;

	// TODO: check whether to redraw DATE & WEEK

	BRect r = LBView::Bounds();
	r.top = 14;
	r.bottom -= 13;
	InvalidRect(r);

	if(fTabIndex == 0 && real_time_clock_usecs() - fShowTimestamp > 2000000) // 2s
	{
		HideNavButton(0);
		HideNavButton(2);
	}
}


void
TMainPageView::DrawClock(BRect rect)
{
	char buf[128];
	uint16 w;

	time_t timer = (time_t)(real_time_clock_usecs() / (bigtime_t)1000000);
	struct tm t;
	if(localtime_r(&timer, &t) == NULL) return;

	BRect r = LBView::Bounds();
	r.bottom = 13;
	if(r.Intersects(rect))
	{
		snprintf(buf, sizeof(buf), "%d年%d月%d日",
			 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday);
		SetFontSize(12);
		w = StringWidth(buf);
		DrawString(buf, BPoint(r.Center().x - w / 2.f, 1));
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom - 13;
	if(r.Intersects(rect))
	{
		FillRect(r & rect);

		// TODO: AM/PM
		if(fShowSeconds)
			snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
				 f24Hours ? t.tm_hour : (t.tm_hour % 12),
				 t.tm_min, t.tm_sec);
		else
			snprintf(buf, sizeof(buf), "%02d:%02d",
				 f24Hours ? t.tm_hour : (t.tm_hour % 12),
				 t.tm_min);
		SetFontSize(fShowSeconds ? 24 : 32);
		w = StringWidth(buf);
		DrawString(buf, r.Center() - BPoint(w / 2.f, (fShowSeconds ? 23 : 31) / 2.f), true);
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom;
	if(r.Intersects(rect))
	{
		const char *desc[] = {"日", "一", "二", "三", "四", "五", "六"};
		snprintf(buf, sizeof(buf), "星期%s", desc[t.tm_wday]);
		SetFontSize(12);
		w = StringWidth(buf);
		DrawString(buf, BPoint(r.Center().x - w / 2.f, r.top + 1));
	}
}


void
TMainPageView::DrawBoardInfo(BRect rect)
{
#if 1
	// TEST
	int k;
	BPoint pt(0, 0);

	for(k = 0; k <= LBK_ICON_RIGHT; k++)
	{
		DrawIcon((lbk_icon_id)k, pt);
		pt.x += 17;
		if(pt.x + 16 > LBView::Bounds().right)
		{
			pt.x = 0;
			pt.y += 17;
		}
	}

	pt.Set(0, 33);
	for(k = LBK_ICON_WARNING; k <= LBK_ICON_POWER_OFF; k++)
	{
		DrawIcon((lbk_icon_id)k, pt);
		pt.x += 33;
	}
#endif
}


void
TMainPageView::DrawClientsInfo(BRect rect)
{
#if 1
	// TEST
	BString aStr("Nothing yet");

	SetFontSize(16);

	uint16 w = StringWidth(aStr.String());
	DrawString(aStr.String(),
		   LBView::Bounds().Center() - BPoint(w / 2.f, 15 / 2.f));
#endif
}


void
TMainPageView::Draw(BRect updateRect)
{
	LBPageView::Draw(updateRect);

	if(fTabIndex == 0)
	{
		DrawClock(updateRect);
	}
	else
	{
		updateRect &= Bounds();

		if(fTabIndex == -1)
			DrawBoardInfo(updateRect);
		else
			DrawClientsInfo(updateRect);
	}
}


void
TMainPageView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	if(clicks == 0xff) // long press
	{
		LBView *view = FindStickView("PowerOffRequested");
		if(view == NULL)
		{
			view = new LBAlertView("关机",
						 "是否确定\n进行关机操作?",
						 LBK_ICON_NO, LBK_ICON_YES, LBK_ICON_NONE,
						 B_WARNING_ALERT);
			BInvoker *invoker = new BInvoker(new BMessage(POWER_REQUESTED_CONFIRM_MSG), this);
			cast_as(view, LBAlertView)->SetInvoker(invoker);
			view->SetName("PowerOffRequested");
			AddStickView(view);
		}
		view->StandIn();
		printf("[TMainPageView]: Power off requested.\n");
	}

	fShowTimestamp = real_time_clock_usecs();
	if(fTabIndex > -1)
		ShowNavButton(0);
	else
		HideNavButton(0);

	if(fTabIndex < 1)
		ShowNavButton(2);
	else
		HideNavButton(2);

	// TODO
}


void
TMainPageView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	int32 saveIndex = fTabIndex;

	if(clicks == 1)
	{
		if(key == 0 && fTabIndex > -1) // Left
		{
			fTabIndex--;
		}
		else if(key == 2 && fTabIndex < 1) // Right
		{
			fTabIndex++;
		}
		else if(key == 1) // Setup
		{
			// TODO
		}
	}
	else if(clicks != 0xff)
	{
		if(key == 1) // Home
		{ 
			fTabIndex = 0;
		}
		else if(key == 0) // Left Switch
		{
			SwitchToPrevPage();
			return;
		}
		else if(key == 2) // Right Switch
		{
			SwitchToNextPage();
			return;
		}
	}

	if(saveIndex != fTabIndex)
	{
		if(fTabIndex == 0)
			cast_as(Looper(), LBApplication)->SetPulseRate(fShowSeconds ? 1000000 : 10000000);
		else
			cast_as(Looper(), LBApplication)->SetPulseRate(0);

		fShowTimestamp = real_time_clock_usecs();
		if(fTabIndex > -1)
			ShowNavButton(0);
		else
			HideNavButton(0);

		if(fTabIndex < 1)
			ShowNavButton(2);
		else
			HideNavButton(2);

		InvalidRect();
	}
}


void
TMainPageView::Set24Hours(bool state)
{
	if(f24Hours != state)
	{
		f24Hours = state;
		InvalidRect();
	}
}


void
TMainPageView::ShowSeconds(bool state)
{
	if(fShowSeconds != state)
	{
		fShowSeconds = state;
		InvalidRect();
	}
}


void
TMainPageView::Activated(bool state)
{
	LBPageView::Activated(state);

	if(state == false)
		cast_as(Looper(), LBApplication)->SetPulseRate(0);
}


void
TMainPageView::MessageReceived(BMessage *msg)
{
	int32 which = -1;
	uint8 clicks = 0;
	LBView *view;

	switch(msg->what)
	{
		case POWER_REQUESTED_CONFIRM_MSG:
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(msg->FindInt32("which", &which) != B_OK) break;
			if(clicks > 1) break;
			view = FindStickView("PowerOffRequested");
			if(view == NULL) break;
			view->StandBack();
			if(which != 1) break;
			printf("[TMainPageView]: Going to power off !\n");
			// TODO: Show "Shuting down..."
			system("poweroff");
			break;

		default:
			LBPageView::MessageReceived(msg);
	}
}


