/* --------------------------------------------------------------------------
 *
 * Little Board Application Kit
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
 * File: LBPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKConfig.h>
#include <lbk/LBApplication.h>
#include <lbk/LBPageView.h>

#define ICON_IS_16x16(id)	((id) > LBK_ICON_ID_16x16_BEGIN && (id) < LBK_ICON_ID_16x16_END)
#define ICON_IS_VALID(id)	((id) == LBK_ICON_NONE || ICON_IS_16x16(id))


LBPageView::LBPageView(const char *name)
	: LBView(name),
	  fNavButtonsState(0)
{
	memset(fButtonIcons, LBK_ICON_NONE, sizeof(fButtonIcons));
}


LBPageView::~LBPageView()
{
}


void
LBPageView::ShowNavButton(uint8 idBtn)
{
	if(idBtn >= LBK_KEY_TYPICAL_NUMBER) return;

	if((fNavButtonsState & (0x01 << idBtn)) == 0)
	{
		fNavButtonsState |= (0x01 << idBtn);

		// It's unnesssary to draw single icon, here we draw all
		BRect r = LBView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
LBPageView::HideNavButton(uint8 idBtn)
{
	if(idBtn >= LBK_KEY_TYPICAL_NUMBER) return;

	if((fNavButtonsState & (0x01 << idBtn)) != 0)
	{
		fNavButtonsState &= ~(0x01 << idBtn);

		// It's unnesssary to draw single icon, here we draw all
		BRect r = LBView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


bool
LBPageView::IsNavButtonHidden(uint8 idBtn) const
{
	if(idBtn >= LBK_KEY_TYPICAL_NUMBER) return false;

	return((fNavButtonsState & (0x01 << idBtn)) == 0);
}


void
LBPageView::SetNavButtonIcon(int32 idBtn, lbk_icon_id idIcon)
{
	if(idBtn >= LBK_KEY_TYPICAL_NUMBER) return;
	if(!ICON_IS_VALID(idIcon)) return;

	if(fButtonIcons[idBtn] != idIcon)
	{
		fButtonIcons[idBtn] = idIcon;
		if(IsNavButtonHidden(idBtn) == false)
		{
			// It's unnesssary to draw single icon, here we draw all
			BRect r = LBView::Bounds();
			r.top = r.bottom - 15;

			InvalidRect(r);
		}
	}
}


lbk_icon_id
LBPageView::GetNavButtonIcon(int32 idBtn) const
{
	return(IsNavButtonHidden(idBtn) ? LBK_ICON_NONE : fButtonIcons[idBtn]);
}


void
LBPageView::DrawNavButtonIcon(lbk_icon_id idIcon, BPoint location)
{
	// for LBK_KEY_TYPICAL_NUMBER <= 2, so on
	DrawIcon(idIcon, location);
}


BRect
LBPageView::Bounds() const
{
	BRect r = LBView::Bounds();

	if(fNavButtonsState != 0)
		r.bottom -= 16;

	return r;
}


void
LBPageView::Draw(BRect rect)
{
	if(fNavButtonsState == 0) return;

	BRect r = LBView::Bounds();
	r.right = r.Width() / (float)LBK_KEY_TYPICAL_NUMBER - 1.f;
	r.top = r.bottom - 15;

	for(int k = 0; k < LBK_KEY_TYPICAL_NUMBER; k++)
	{
		if(IsNavButtonHidden(k) == false && r.Intersects(rect))
		{
			BPoint pt = r.Center() - BPoint(7, 7);
			uint8 pressed = 0;

			KeyState(&pressed);
			if(pressed & (0x01 << k)) pt += BPoint(1, 1);

			DrawNavButtonIcon(fButtonIcons[k], pt);
		}
		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
LBPageView::KeyDown(uint8 key, uint8 clicks)
{
	if(clicks == 1 && IsNavButtonHidden(key) == false)
	{
		// It's unnesssary to draw single icon, here we draw all
		BRect r = LBView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
LBPageView::KeyUp(uint8 key, uint8 clicks)
{
	if(IsNavButtonHidden(key) == false)
	{
		// It's unnesssary to draw single icon, here we draw all
		BRect r = LBView::Bounds();
		r.top = r.bottom - 15;

		InvalidRect(r);
	}
}


void
LBPageView::SwitchToNextPage()
{
	LBApplication *app = (Looper() ? cast_as(Looper(), LBApplication) : NULL);
	if(app == NULL || MasterView() != NULL || IsActivated() == false) return;

	if(IsFarRightPage()) return;

	// Left side: 4 3 2 1 0
	int32 count = app->CountPageViews(true);
	for(int32 k = count - 1; k >= 0; k--)
	{
		LBView *view = app->PageViewAt(k, true);
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
		LBView *view = app->PageViewAt(k, false);
		if(view != this) continue;
		app->ActivatePageView(k + 1, false);
	}
}


void
LBPageView::SwitchToPrevPage()
{
	LBApplication *app = (Looper() ? cast_as(Looper(), LBApplication) : NULL);
	if(app == NULL || MasterView() != NULL || IsActivated() == false) return;

	if(IsFarLeftPage()) return;

	// Right side: 0 1 2 3 4
	int32 count = app->CountPageViews(false);
	for(int32 k = count - 1; k >= 0; k--)
	{
		LBView *view = app->PageViewAt(k, false);
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
		LBView *view = app->PageViewAt(k, true);
		if(view != this) continue;
		app->ActivatePageView(k + 1, true);
	}
}


bool
LBPageView::IsFarLeftPage() const
{
	if(MasterView() != NULL)
	{
		LBPageView *view = cast_as(TopView(), LBPageView);
		return(view == NULL ? false : view->IsFarLeftPage());
	}

	LBApplication *app = (Looper() ? cast_as(Looper(), LBApplication) : NULL);
	if(app == NULL) return false;

	int32 count = app->CountPageViews(true);
	if(count > 0)
		return(app->PageViewAt(count - 1, true) == this);
	else
		return(app->PageViewAt(0, false) == this);
}


bool
LBPageView::IsFarRightPage() const
{
	if(MasterView() != NULL)
	{
		LBPageView *view = cast_as(TopView(), LBPageView);
		return(view == NULL ? false : view->IsFarRightPage());
	}

	LBApplication *app = (Looper() ? cast_as(Looper(), LBApplication) : NULL);
	if(app == NULL) return false;

	int32 count = app->CountPageViews(false);
	if(count > 0)
		return(app->PageViewAt(count - 1, false) == this);
	else
		return(app->PageViewAt(0, true) == this);
}

