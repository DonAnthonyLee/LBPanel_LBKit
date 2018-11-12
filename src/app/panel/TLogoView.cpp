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
 * File: TLogoView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "TLogoView.h"


TLogoView::TLogoView(const char *name)
	: LBView(name), fTicks(0)
{
}


TLogoView::~TLogoView()
{
}


void
TLogoView::KeyUp(uint8 key, uint8 clicks)
{
	StandBack();
}


void
TLogoView::StandIn()
{
	LBView *master = MasterView();
	if(master == NULL) return;

	LBView *old = master->StandingInView();
	if(old == this) return;

	LBView::StandIn();
	if(master->StandingInView() != this) return;

	fTicks = 0;
}


void
TLogoView::Draw(BRect updateRect)
{
	BString str("LBPanel");
	BRect r;
	BPoint pt;
	uint16 w;

	r = Bounds();
	r.top = r.top + r.Height() / 2.f - 15;
	r.bottom = r.top + 30;
	FillRect(r, B_SOLID_LOW);

	SetFontSize(24);
	w = StringWidth(str.String());
	pt = r.LeftTop() + BPoint(r.Width() / 2.f, r.Height() / 2.f) - BPoint(w / 2.f, 23 / 2.f);
	DrawString(str.String(), pt);

	r.top = r.bottom - r.Height() / 2.f + 1;
	InvertRect(r);

	r.OffsetBy(0, -(r.Height() + 1));
	r.right = r.left + 8 * fTicks;
	InvertRect(r);
}


void
TLogoView::Pulse()
{
	// TODO: reading settings, etc.

	fTicks++;
	EnableUpdate(false);
	Draw(Bounds());
	EnableUpdate(true);

	if(fTicks < (Bounds().Width() / 8) + 5) return;
	StandBack();
}

