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

#include "TMainPageView.h"


TMainPageView::TMainPageView(const char *name)
	: OLEDPageView(0, name), fTabIndex(1), f24Hours(true), fShowSeconds(true)
{
	// TODO
}

TMainPageView::~TMainPageView()
{
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
		snprintf(buf, sizeof(buf), "%d 年 %d 月 %d 日",
			 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday);
		SetFontSize(12);
		w = StringWidth(buf);
		FillRect(r, B_SOLID_LOW);
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
		FillRect(r, B_SOLID_LOW);
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
		FillRect(r, B_SOLID_LOW);
		DrawString(buf, BPoint(r.Center().x - w / 2.f, r.top + 1));
	}
}


void
TMainPageView::DrawBoardInfo(BRect rect)
{
	BString aStr("Nothing yet <");

	FillRect(rect, B_SOLID_LOW);
	SetFontSize(16);

	uint16 w = StringWidth(aStr.String());
	DrawString(aStr.String(),
		   rect.Center() - BPoint(w / 2.f, 15 / 2.f));
}


void
TMainPageView::DrawClientsInfo(BRect rect)
{
	BString aStr("> Nothing yet");

	FillRect(rect, B_SOLID_LOW);
	SetFontSize(16);

	uint16 w = StringWidth(aStr.String());
	DrawString(aStr.String(),
		   rect.Center() - BPoint(w / 2.f, 15 / 2.f));
}


void
TMainPageView::Draw(BRect updateRect)
{
	if(fTabIndex == 1)
	{
		DrawClock(updateRect);
	}
	else
	{
		OLEDPageView::Draw(updateRect);
		updateRect &= Bounds();

		if(fTabIndex == 0)
			DrawBoardInfo(updateRect);
		else
			DrawClientsInfo(updateRect);
	}
}


void
TMainPageView::KeyDown(uint8 key, uint8 clicks)
{
	// TODO
}


void
TMainPageView::KeyUp(uint8 key, uint8 clicks)
{
	int32 saveIndex = fTabIndex;

	if(clicks == 1)
	{
		int32 saveIndex = fTabIndex;
		if(key == 0 && fTabIndex > 0) // Up
		{
			fTabIndex--;
		}
		else if(key == 2 && fTabIndex < 2) // Down
		{
			fTabIndex++;
		}
		else if(key == 1) // Setup
		{
			// TODO
		}

		if(saveIndex != fTabIndex)
		{
			EnableUpdate(false);
			FillRect(OLEDView::Bounds(), B_SOLID_LOW);
			Draw(OLEDView::Bounds());
			EnableUpdate(true);
		}
	}
	else
	{
		if(key == 1) // Home
		{ 
			fTabIndex = 1;
		}
		else if(key == 0) // Left
		{
			// TODO
		}
		else if(key == 2) // Right
		{
			// TODO
		}
	}

	if(saveIndex != fTabIndex)
	{
		EnableUpdate(false);
		FillRect(OLEDView::Bounds(), B_SOLID_LOW);
		Draw(OLEDView::Bounds());
		EnableUpdate(true);
	}
}

