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
 * File: OLEDPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "OLEDConfig.h"
#include "OLEDApp.h"
#include "OLEDPageView.h"


OLEDPageView::OLEDPageView(const char *name)
	: OLEDView(name),
	  fNavButtonsState(0)
{
}


OLEDPageView::~OLEDPageView()
{
}


void
OLEDPageView::ShowNavButton(uint8 idBtn)
{
	if(idBtn >= OLED_BUTTONS_NUM) return;

	if((fNavButtonsState & (0x01 << idBtn)) == 0)
	{
		fNavButtonsState |= (0x01 << idBtn);

		// It's unnesssary to draw single icon, here we draw all
		BRect r = OLEDView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
OLEDPageView::HideNavButton(uint8 idBtn)
{
	if(idBtn >= OLED_BUTTONS_NUM) return;

	if((fNavButtonsState & (0x01 << idBtn)) != 0)
	{
		fNavButtonsState &= ~(0x01 << idBtn);

		// It's unnesssary to draw single icon, here we draw all
		BRect r = OLEDView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


bool
OLEDPageView::IsNavButtonHidden(uint8 idBtn) const
{
	if(idBtn >= OLED_BUTTONS_NUM) return false;

	return((fNavButtonsState & (0x01 << idBtn)) == 0);
}


void
OLEDPageView::SetNavButtonIcon(int32 idBtn, oled_icon_id idIcon)
{
	if(idBtn >= OLED_BUTTONS_NUM) return;

	if(fButtonIcons[idBtn] != idIcon)
	{
		fButtonIcons[idBtn] = idIcon;
		if(IsNavButtonHidden(idBtn) == false)
		{
			// It's unnesssary to draw single icon, here we draw all
			BRect r = OLEDView::Bounds();
			r.top = r.bottom - 15;

			InvalidRect(r);
		}
	}
}


oled_icon_id
OLEDPageView::GetNavButtonIcon(int32 idBtn) const
{
	return(IsNavButtonHidden(idBtn) ? OLED_ICON_NONE : fButtonIcons[idBtn]);
}


void
OLEDPageView::DrawNavButton(oled_icon_id idIcon, BPoint location)
{
	DrawIcon(idIcon, location);
}


BRect
OLEDPageView::Bounds() const
{
	BRect r = OLEDView::Bounds();

	if(fNavButtonsState != 0)
		r.bottom -= 16;

	return r;
}


void
OLEDPageView::Draw(BRect rect)
{
	if(fNavButtonsState == 0) return;

	BRect r = OLEDView::Bounds();
	r.right = r.Width() / (float)OLED_BUTTONS_NUM - 1.f;
	r.top = r.bottom - 15;

	for(int k = 0; k < OLED_BUTTONS_NUM; k++)
	{
		if(IsNavButtonHidden(k) == false && r.Intersects(rect))
		{
			BPoint pt = r.Center() - BPoint(7, 7);
			uint8 pressed = 0;

			KeyState(&pressed);
			if(pressed & (0x01 << k)) pt += BPoint(1, 1);

			DrawNavButton(fButtonIcons[k], pt);
		}
		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
OLEDPageView::KeyDown(uint8 key, uint8 clicks)
{
	if(clicks == 1 && IsNavButtonHidden(key) == false)
	{
		// It's unnesssary to draw single icon, here we draw all
		BRect r = OLEDView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
OLEDPageView::KeyUp(uint8 key, uint8 clicks)
{
	if(IsNavButtonHidden(key) == false)
	{
		// It's unnesssary to draw single icon, here we draw all
		BRect r = OLEDView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
OLEDPageView::SwitchToNextPage()
{
	OLEDApp *app = cast_as(Looper(), OLEDApp);
	if(app == NULL || IsActivated() == false) return;

	if(IsFarRightPage()) return;

	// Left side: 4 3 2 1 0
	int32 count = app->CountPageViews(true);
	for(int32 k = count - 1; k >= 0; k--)
	{
		OLEDView *view = app->PageViewAt(k, true);
		if(view != this) continue;
		if(k > 0)
			app->ActivatePageView(k - 1, true);
		else
			app->ActivatePageView(0, false);
		return;
	}

	// Right side: 0 1 2 3 4
	count = app->CountPageViews(false);
	for(int32 k = 0; k < count; k++)
	{
		OLEDView *view = app->PageViewAt(k, false);
		if(view != this) continue;
		app->ActivatePageView(k + 1, false);
	}
}


void
OLEDPageView::SwitchToPrevPage()
{
	OLEDApp *app = cast_as(Looper(), OLEDApp);
	if(app == NULL || IsActivated() == false) return;

	if(IsFarLeftPage()) return;

	// Right side: 0 1 2 3 4
	int32 count = app->CountPageViews(false);
	for(int32 k = count - 1; k >= 0; k--)
	{
		OLEDView *view = app->PageViewAt(k, false);
		if(view != this) continue;
		if(k > 0)
			app->ActivatePageView(k - 1, false);
		else
			app->ActivatePageView(0, true);
		return;
	}

	// Left side: 4 3 2 1 0
	count = app->CountPageViews(true);
	for(int32 k = 0; k < count; k++)
	{
		OLEDView *view = app->PageViewAt(k, true);
		if(view != this) continue;
		app->ActivatePageView(k + 1, true);
	}
}


bool
OLEDPageView::IsFarLeftPage() const
{
	OLEDApp *app = cast_as(Looper(), OLEDApp);
	if(app == NULL) return false;

	int32 count = app->CountPageViews(true);
	if(count > 0)
		return(app->PageViewAt(count - 1, true) == this);
	else
		return(app->PageViewAt(0, false) == this);
}


bool
OLEDPageView::IsFarRightPage() const
{
	OLEDApp *app = cast_as(Looper(), OLEDApp);
	if(app == NULL) return false;

	int32 count = app->CountPageViews(false);
	if(count > 0)
		return(app->PageViewAt(count - 1, false) == this);
	else
		return(app->PageViewAt(0, true) == this);
}

