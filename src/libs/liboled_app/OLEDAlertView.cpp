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
 * File: OLEDAlertView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "OLEDConfig.h"
#include <OLEDAlertView.h>

OLEDAlertView::OLEDAlertView(const char *title,
			     const char *text,
			     oled_icon_id icon3,
			     oled_icon_id icon2,
			     oled_icon_id icon1,
			     alert_type type)
	: OLEDView(NULL),
	  fTitle(title),
	  fText(text)
{
	fIcons[0] = icon1;
	fIcons[1] = icon2;
	fIcons[2] = icon3;
	switch(type)
	{
		// TODO
		default:
			fIcons[3] = OLED_ICON_NONE;
	}
}


OLEDAlertView::~OLEDAlertView()
{
}


void
OLEDAlertView::SetTitle(const char *title)
{
	fTitle.Truncate(0);
	fTitle.SetTo(title);
	InvalidRect();
}


void
OLEDAlertView::SetText(const char *text)
{
	fText.Truncate(0);
	fText.SetTo(text);
	InvalidRect();
}


void
OLEDAlertView::SetButtonIcon(int32 index, oled_icon_id icon)
{
	if(index < 0 || index > 2) return;
	if(icon >= OLED_ICON_ID_16x16_MAX) return;

	if(fIcons[index] != icon)
	{
		fIcons[index] = icon;
		InvalidRect();
	}
}


int32
OLEDAlertView::Go()
{
	if(MasterView() != NULL) return -1;

	// TODO
	return -1;
}


status_t
OLEDAlertView::Go(BInvoker *invoker)
{
	if(invoker == NULL) return B_BAD_VALUE;
	if(MasterView() != NULL) return B_BAD_TYPE;

	// TODO
	return B_ERROR;
}

