/* --------------------------------------------------------------------------
 *
 * Panel Application for NanoPi OLED Hat
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

#include <OLEDApp.h>
#include "TMainPageView.h"


TMainPageView::TMainPageView(const char *name)
	: OLEDPageView(name), fTabIndex(0), f24Hours(true), fShowSeconds(false), fShowTimestamp(0)
{
	SetNavButtonIcon(0, OLED_ICON_UP);
	SetNavButtonIcon(2, OLED_ICON_DOWN);

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

	BRect r = OLEDView::Bounds();
	r.top = 14;
	r.bottom -= 13;
	InvalidRect(r);

	if(fTabIndex == 0 && real_time_clock_usecs() - fShowTimestamp > 3000000) // 3s
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

	BRect r = OLEDView::Bounds();
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
	r.bottom = OLEDView::Bounds().bottom - 13;
	if(r.Intersects(rect))
	{
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
		DrawString(buf, r.Center() - BPoint(w / 2.f, (fShowSeconds ? 23 : 31) / 2.f));
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = OLEDView::Bounds().bottom;
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

	for(k = 0; k <= OLED_ICON_RIGHT; k++)
	{
		DrawIcon((oled_icon_id)k, pt);
		pt.x += 17;
		if(pt.x + 16 > OLEDView::Bounds().right)
		{
			pt.x = 0;
			pt.y += 17;
		}
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
		   OLEDView::Bounds().Center() - BPoint(w / 2.f, 15 / 2.f));
#endif
}


void
TMainPageView::Draw(BRect updateRect)
{
	OLEDPageView::Draw(updateRect);

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
	OLEDPageView::KeyDown(key, clicks);

	if(clicks == 0xff) // long press
	{
		// TODO: confirm to power off
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
	OLEDPageView::KeyUp(key, clicks);

	int32 saveIndex = fTabIndex;

	if(clicks == 1)
	{
		if(key == 0 && fTabIndex > -1) // Up
		{
			fTabIndex--;
		}
		else if(key == 2 && fTabIndex < 1) // Down
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
		else if(key == 0) // Left
		{
			SwitchToPrevPage();
			return;
		}
		else if(key == 2) // Right
		{
			SwitchToNextPage();
			return;
		}
	}

	if(saveIndex != fTabIndex)
	{
		if(fTabIndex == 0)
			cast_as(Looper(), OLEDApp)->SetPulseRate(fShowSeconds ? 1000000 : 10000000);
		else
			cast_as(Looper(), OLEDApp)->SetPulseRate(0);

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
		if(IsActivated())
			InvalidRect();
	}
}


void
TMainPageView::ShowSeconds(bool state)
{
	if(fShowSeconds != state)
	{
		fShowSeconds = state;
		if(IsActivated())
			InvalidRect();
	}
}

