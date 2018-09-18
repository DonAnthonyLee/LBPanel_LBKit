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
#include "OLEDPageView.h"


OLEDPageView::OLEDPageView(uint8 num, const char *name)
	: OLEDView(name),
	  fNavButtonsState(0)
{
	num = min_c(OLED_BUTTONS_NUM, num);
	switch(num)
	{
		case 1:
			fButtonIcons[1] = OLED_ICON_OK;
			fNavButtonsState = (0x01 << 1);
			break;

		case 2:
			fButtonIcons[0] = OLED_ICON_YES;
			fButtonIcons[2] = OLED_ICON_NO;
			fNavButtonsState = (0x01 << 0) | (0x01 << 2);
			break;

		case 3:
			fButtonIcons[0] = OLED_ICON_UP;
			fButtonIcons[1] = OLED_ICON_OK;
			fButtonIcons[2] = OLED_ICON_DOWN;
			fNavButtonsState = 0x07;
			break;

		default:
			break;
	}
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
		// TODO
	}
}


void
OLEDPageView::HideNavButton(uint8 idBtn)
{
	if(idBtn >= OLED_BUTTONS_NUM) return;

	if((fNavButtonsState & (0x01 << idBtn)) != 0)
	{
		fNavButtonsState &= ~(0x01 << idBtn);
		// TODO
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
		// TODO
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
	// TODO
}


BRect
OLEDPageView::Bounds() const
{
	BRect r = OLEDView::Bounds();

	// TODO

	return r;
}


void
OLEDPageView::Draw(BRect r)
{
	// TODO
}


void
OLEDPageView::KeyDown(uint8 key, uint8 clicks)
{
	// TODO
}


void
OLEDPageView::KeyUp(uint8 key, uint8 clicks)
{
	// TODO
}


void
OLEDPageView::Activated(bool state)
{
	// TODO
}

