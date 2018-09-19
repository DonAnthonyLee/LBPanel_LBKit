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
 * File: OLEDView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include <oled_ssd1306_ioctl.h>

#include "OLEDConfig.h"
#include "OLEDView.h"

OLEDView::OLEDView(const char *name)
	: BHandler(name),
	  fFD(-1), fTimestamp(0),
	  fActivated(false),
	  fFontSize(12),
	  fKeyState(0)
{
}


OLEDView::~OLEDView()
{
}


void
OLEDView::FillRect(BRect r, pattern p)
{
	_oled_ssd1306_clear_t data;

	if (fFD < 0 || fActivated == false || r.IsValid() == false) return;

	data.x = (uint8_t)r.left;
	data.y = (uint8_t)r.top;
	data.w = (uint8_t)(r.Width() + 1);
	data.h = (uint8_t)(r.Height() + 1);
	bzero(data.patterns, sizeof(data.patterns));
	for(int k = 0; k < 8; k++)
	{
		if(p.data[k] == 0x00 || p.data[k] == 0xff)
			data.patterns[k] = p.data[k];
		else for(int m = 0; m < 8; m++)
			data.patterns[k] |= ((p.data[m] >> (7 - k + m)) & (0x01 << m));
	}

	if(ioctl(fFD, OLED_SSD1306_IOC_CLEAR, &data) == 0) fTimestamp = data.ts;
}


void
OLEDView::StrokeRect(BRect rect, bool erase)
{
	BRect r;
	pattern p = (erase ? B_SOLID_LOW : B_SOLID_HIGH);

	if (fFD < 0 || fActivated == false || r.IsValid() == false) return;

	r = rect;
	r.bottom = r.top;
	FillRect(r, p);
	if (rect.Height() > 0)
	{
		r.top = r.bottom = rect.bottom;
		FillRect(r, p);
	}

	rect.InsetBy(0, 1);
	if (rect.IsValid() == false) return;

	r = rect;
	r.left = r.right;
	FillRect(r, p);
	if (rect.Width() > 0)
	{
		r.right = r.left = rect.left;
		FillRect(r, p);
	}
}


void
OLEDView::DrawString(const char *str, BPoint pt, bool erase)
{
	_oled_ssd1306_show_t data;

	if (fFD < 0 || fActivated == false) return;

	data.x = (uint8_t)pt.x;
	data.y = (uint8_t)pt.y;
	data.size = fFontSize;
	strncpy(data.str, str, sizeof(data.str));
	data.erase_mode = erase ? 1 : 0;

	if(ioctl(fFD, OLED_SSD1306_IOC_SHOW, &data) == 0) fTimestamp = data.ts;
}


void
OLEDView::DrawIcon(const oled_icon *icon, BPoint pt)
{
	_oled_ssd1306_clear_t data;

	if (fFD < 0 || fActivated == false) return;
	if (icon == NULL || icon->type > 2 /*OLED_ICON_32x32*/) return;

	data.w = 8;
	data.h = 8;
	for(int k = 0; k < (1 << icon->type); k++) // y
	{
		for(int m = 0; m < (1 << icon->type); m++) // x
		{
			data.x = (m << 3) + (uint8_t)pt.x;
			data.y = (k << 3) + (uint8_t)pt.y;
			memcpy(data.patterns,
			       icon->data + ((k * (1 << icon->type) + m) << 3),
			       sizeof(data.patterns));

			if(ioctl(fFD, OLED_SSD1306_IOC_CLEAR, &data) == 0) fTimestamp = data.ts;
		}
	}
}


void
OLEDView::DrawIcon(oled_icon_id id, BPoint pt)
{
	DrawIcon(oled_get_icon_data(id), pt);
}


uint8
OLEDView::FontSize() const
{
	return fFontSize;
}


void
OLEDView::SetFontSize(uint8 size)
{
	if(!(size == 12 || size == 14 || size == 16 || size == 24 || size == 32)) return;
	fFontSize = size;
}


uint16
OLEDView::StringWidth(const char *str) const
{
	_oled_ssd1306_string_width_t data;

	if (fFD < 0) return 0;

	data.w = data.h = 0;
	data.size = fFontSize;
	strncpy(data.str, str, sizeof(data.str));

	return((ioctl(fFD, OLED_SSD1306_IOC_SHOW, &data) == 0) ? data.w : 0);
}


void
OLEDView::EnableUpdate(bool state)
{
	uint8_t st = state;

	if (fFD < 0 || fActivated == false) return;
	ioctl(fFD, OLED_SSD1306_IOC_UPDATE, &st);
}


bool
OLEDView::IsNeededToRegen() const
{
	_oled_ssd1306_get_ts_t data;

	if (fFD < 0 || fActivated == false) return false;

	data.last_action = 0;
	if(ioctl(fFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return false;

	return(data.ts > fTimestamp);
}


bool
OLEDView::GetPowerState() const
{
	_oled_ssd1306_power_t data;

	if (fFD < 0) return false;

	data.state = 2;
	if(ioctl(fFD, OLED_SSD1306_IOC_POWER, &data) != 0) return false;
	return(data.state == 1);
}


void
OLEDView::SetPowerState(bool state)
{
	_oled_ssd1306_power_t data;

	if (fFD < 0 || fActivated == false) return;

	data.state = (state ? 1 : 0);
	if(ioctl(fFD, OLED_SSD1306_IOC_POWER, &data) == 0) fTimestamp = data.ts;
}


BRect
OLEDView::Bounds() const
{
	return BRect(0, 0, OLED_SCREEN_WIDTH - 1, OLED_SCREEN_HEIGHT - 1);
}


void
OLEDView::Draw(BRect updateRect)
{
	// Empty
}


void
OLEDView::KeyDown(uint8 key, uint8 clicks)
{
	// Empty
}


void
OLEDView::KeyUp(uint8 key, uint8 clicks)
{
	// Empty
}


uint8
OLEDView::KeyState(bool *pressed) const
{
	if(pressed) *pressed = (fKeyState & (0x01 << 8)) == 0 ? false : true;
	return fKeyState;
}

void
OLEDView::MessageReceived(BMessage *msg)
{
	// TODO: handling key events from app's looper
	switch(msg->what)
	{
		default:
			break;
	}
}


void
OLEDView::SetActivated(bool state)
{
	if(fActivated != state)
	{
		fActivated = state;
		Activated(state);
	}
}


bool
OLEDView::IsActivated() const
{
	return fActivated;
}


void
OLEDView::Activated(bool state)
{
	if(state) Draw(OLEDView::Bounds());
}

