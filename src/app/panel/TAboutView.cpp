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
 * File: TAboutView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "TAboutView.h"
#include "TMainPageView.h" // for "LBPANEL_xxxx_VERSION"


TAboutView::TAboutView(const char *name)
	: LBView(name)
{
	// TODO
}


TAboutView::~TAboutView()
{
	// TODO
}


void
TAboutView::KeyUp(uint8 key, uint8 clicks)
{
	StandBack();
}


void
TAboutView::Attached()
{
	LBView::Attached();
	StandIn();
}


void
TAboutView::Draw(BRect rect)
{
	BString str;
	BRect r;

	SetFontSize(12);

	// Name
	r = Bounds();
	r.bottom = r.top + r.Height() / 4.f - 1.f;
	str.SetTo("应用: LBPanel");
	DrawString(str.String(), r.LeftTop() + BPoint(5, (r.Height() - 11.f) / 2.f));

	// Version
	r.OffsetBy(0, r.Height() + 1);
	str.Truncate(0);
	str << "版本: " << LBPANEL_MAJOR_VERSION << "." << LBPANEL_MINOR_VERSION;
	DrawString(str.String(), r.LeftTop() + BPoint(5, (r.Height() - 11.f) / 2.f));

	// Author
	r.OffsetBy(0, r.Height() + 1);
	str.Truncate(0);
	str.SetTo("作者: Anthony Lee");
	DrawString(str.String(), r.LeftTop() + BPoint(5, (r.Height() - 11.f) / 2.f));

	// Copyright
	r.OffsetBy(0, r.Height() + 1);
	str.Truncate(0);
	str.SetTo("协议: MIT");
	DrawString(str.String(), r.LeftTop() + BPoint(5, (r.Height() - 11.f) / 2.f));
}

