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
#define POWER_OFF_MSG			'pwOF'


TMainPageView::TMainPageView(const char *name)
	: LBPageView(name), fTabIndex(0), f24Hours(false), fShowSeconds(false), fShowTimestamp(0)
{
	SetNavButtonIcon(0, LBK_ICON_LEFT);
	SetNavButtonIcon(2, LBK_ICON_RIGHT);

	// TODO
}

TMainPageView::~TMainPageView()
{
}


void
TMainPageView::GetTime(BString *strDate, BString *strTime, BString *strWeek) const
{
	time_t timer = (time_t)(real_time_clock_usecs() / (bigtime_t)1000000);
	struct tm t;
	if(localtime_r(&timer, &t) == NULL) return;

	if(strDate != NULL)
	{
		strDate->Truncate(0);
		*strDate << 1900 + t.tm_year << "年" << 1 + t.tm_mon << "月" << t.tm_mday << "日";
	}

	if(strTime != NULL)
	{
		strTime->Truncate(0);
		if(fShowSeconds == false && f24Hours == false)
		{
			if(t.tm_hour < 12)
				*strTime << (t.tm_hour < 6 ? "凌晨" : "上午");
			else
				*strTime << (t.tm_hour < 6 ? "下午" : "晚上");
		}

		*strTime << (f24Hours ? t.tm_hour : (t.tm_hour % 12));
		*strTime << ":" << t.tm_min;
		if(fShowSeconds)
			*strTime << ":" << t.tm_sec;
	}

	if(strWeek != NULL)
	{
		const char *desc[] = {"日", "一", "二", "三", "四", "五", "六"};

		strWeek->SetTo("星期");
		strWeek->Append(desc[t.tm_wday]);
	}
}


void
TMainPageView::Pulse()
{
	BRect r;
	BString strDate, strTime, strWeek;

	if(fTabIndex != 0) return;

	GetTime(&strDate, &strTime, &strWeek);

	if(strDate != fDate)
	{
		fDate = strDate;

		r = LBView::Bounds();
		r.bottom = 13;
		InvalidRect(r);
	}

	if(strTime != fTime)
	{
		fTime = strTime;

		r = LBView::Bounds();
		r.top = 14;
		r.bottom -= 14;
		InvalidRect(r);
	}

	if(strWeek != fWeek)
	{
		fWeek = strWeek;

		r = LBView::Bounds();
		r.top = r.bottom - 13;
		InvalidRect(r);
	}

	if(fShowTimestamp == 0 || real_time_clock_usecs() - fShowTimestamp < 2000000) return;
	fShowTimestamp = 0;

	cast_as(Looper(), LBApplication)->SetPulseRate(fShowSeconds ? 1000000 : 10000000);
	HideNavButton(0);
	HideNavButton(2);
}


void
TMainPageView::DrawClock(BRect rect)
{
	uint16 w;

	time_t timer = (time_t)(real_time_clock_usecs() / (bigtime_t)1000000);
	struct tm t;
	if(localtime_r(&timer, &t) == NULL) return;

	BRect r = LBView::Bounds();
	r.bottom = 13;
	if(r.Intersects(rect) && fDate.Length() > 0)
	{
		SetFontSize(12);
		w = StringWidth(fDate.String());
		DrawString(fDate.String(), BPoint(r.Center().x - w / 2.f, 1));
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom - 14;
	if(r.Intersects(rect) && fTime.Length() > 0)
	{
		FillRect(r & rect);

		SetFontSize(fShowSeconds ? 24 : 32);
		if(fShowSeconds == false && f24Hours == false && fTime.Length() > 6)
		{
			uint16 w1;
			BString str(fTime, 6);

			SetFontSize(12);
			w1 = StringWidth(str.String());
			SetFontSize(32);
			w = w1 + 2 + StringWidth(fTime.String() + 6);

			BPoint pt = r.Center() - BPoint(w / 2.f, 23 / 2.f);
			DrawString(fTime.String() + 6, pt + BPoint(w1 + 2, 0), true);
			SetFontSize(12);
			DrawString(str.String(), pt + BPoint(0, 12), true);
		}
		else
		{
			SetFontSize(fShowSeconds ? 24 : 32);
			w = StringWidth(fTime.String());
			DrawString(fTime.String(), r.Center() - BPoint(w / 2.f, (fShowSeconds ? 23 : 31) / 2.f), true);
		}
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom;
	if(r.Intersects(rect) && fWeek.Length() > 0)
	{
		SetFontSize(12);
		w = StringWidth(fWeek.String());
		DrawString(fWeek.String(), BPoint(r.Center().x - w / 2.f, r.top + 1));
	}
}


void
TMainPageView::DrawBoardInfo(BRect rect)
{
#if 1
	// TEST
	int k;
	BPoint pt(0, 0);

	for(k = LBK_ICON_ID_16x16_BEGIN + 1; k < LBK_ICON_ID_16x16_END; k++)
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
	for(k = LBK_ICON_ID_32x32_BEGIN + 1; k < LBK_ICON_ID_32x32_END; k++)
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

	if(clicks == 0xff && key == 1) // K2 long press
	{
		LBAlertView *view = new LBAlertView("关机",
						    "是否确定\n进行关机操作?",
						    LBK_ICON_NO, LBK_ICON_YES, LBK_ICON_NONE,
						    B_WARNING_ALERT);
		view->SetInvoker(new BInvoker(new BMessage(POWER_REQUESTED_CONFIRM_MSG), this));
		view->SetName("PowerOffRequested");
		AddStickView(view);
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
			cast_as(Looper(), LBApplication)->SetPulseRate(500000);
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
	else
		GetTime(&fDate, &fTime, &fWeek);
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
			if(which != 1)
			{
				RemoveStickView(view);
				delete view;
				break;
			}

			view = new LBAlertView("关机",
					       "正在关机...",
					       LBK_ICON_NONE, LBK_ICON_NONE, LBK_ICON_NONE,
					       B_EMPTY_ALERT);
			AddStickView(view);
			view->StandIn();

			Looper()->PostMessage(POWER_OFF_MSG, this);
			break;

		case POWER_OFF_MSG:
			if(msg->HasBool("delay") == false) // in order to show the "Shuting down..."
			{
				msg->AddBool("delay", true);
				Looper()->PostMessage(msg, this);
				break;
			}

			printf("[TMainPageView]: Going to power off !\n");
			system("poweroff");
			break;

		default:
			LBPageView::MessageReceived(msg);
	}
}


