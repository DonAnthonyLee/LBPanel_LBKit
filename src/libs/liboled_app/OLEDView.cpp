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
#include <OLEDView.h>

OLEDView::OLEDView(const char *name)
	: BHandler(name),
	  fFD(-1), fTimestamp(0),
	  fActivated(false),
	  fFontSize(12),
	  fKeyState(0),
	  fUpdateCount(0),
	  fMasterView(NULL),
	  fStandingInView(NULL)
{
	fUpdateRect = BRect();
}


OLEDView::~OLEDView()
{
	OLEDView *view;
	while((view = (OLEDView*)fStickViews.RemoveItem((int32)0)) != NULL) delete view; 
}


void
OLEDView::FillRect(BRect r, pattern p)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->FillRect(r, p);
		return;
	}

	_oled_ssd1306_clear_t data;

	r &= OLEDView::Bounds();
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
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->StrokeRect(rect, erase);
		return;
	}

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
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->DrawString(str, pt, erase);
		return;
	}

	_oled_ssd1306_show_t data;

	if(str == NULL || *str == 0) return;
	if(fFD < 0 || fActivated == false) return;
	if(pt.x < 0 || pt.y < 0) return;

	bzero(&data, sizeof(data));
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
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->DrawIcon(icon, pt);
		return;
	}

	_oled_ssd1306_clear_t data;

	if(fFD < 0 || fActivated == false) return;
	if(icon == NULL || icon->type > 2 /*OLED_ICON_32x32*/) return;
	if(pt.x < 0 || pt.y < 0) return;

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
	if(fMasterView != NULL)
		return fMasterView->FontSize();

	return fFontSize;
}


void
OLEDView::SetFontSize(uint8 size)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->SetFontSize(size);
		return;
	}

	if(!(size == 12 || size == 14 || size == 16 || size == 24 || size == 32)) return;
	fFontSize = size;
}


uint16
OLEDView::StringWidth(const char *str) const
{
	if(fMasterView != NULL)
		return fMasterView->StringWidth(str);

	_oled_ssd1306_string_width_t data;

	if (fFD < 0 || str == NULL || *str == 0) return 0;

	bzero(&data, sizeof(data));
	data.w = data.h = 0;
	data.size = fFontSize;
	strncpy(data.str, str, sizeof(data.str));

	return((ioctl(fFD, OLED_SSD1306_IOC_STRING_WIDTH, &data) == 0) ? data.w : 0);
}


void
OLEDView::EnableUpdate(bool state)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->EnableUpdate(state);
		return;
	}

	uint8_t st = state;

	if(fFD < 0 || fActivated == false) return;

	if(state == false)
	{
		if(fUpdateCount == 0)
			if(ioctl(fFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return;

		fUpdateCount++;
	}
	else if(fUpdateCount > 0)
	{
		if(fUpdateCount == 1)
			if(ioctl(fFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return;

		fUpdateCount--;
	}
}


bool
OLEDView::IsNeededToRegen() const
{
	if(fMasterView != NULL)
		return fMasterView->IsNeededToRegen();

	_oled_ssd1306_get_ts_t data;

	if(fFD < 0 || fActivated == false) return false;

	data.last_action = 0;
	if(ioctl(fFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return false;

	return(data.ts > fTimestamp);
}


bool
OLEDView::GetPowerState() const
{
	if(fMasterView != NULL)
		return fMasterView->GetPowerState();

	_oled_ssd1306_power_t data;

	if (fFD < 0) return false;

	data.state = 2;
	if(ioctl(fFD, OLED_SSD1306_IOC_POWER, &data) != 0) return false;
	return(data.state == 1);
}


void
OLEDView::SetPowerState(bool state)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->SetPowerState(state);
		return;
	}

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


void
OLEDView::Pulse()
{
	// Empty
}


uint8
OLEDView::KeyState(uint8 *down_state) const
{
	if(fMasterView != NULL)
		return fMasterView->KeyState(down_state);

	if(down_state) *down_state = (fKeyState >> 8);
	return fKeyState;
}

void
OLEDView::MessageReceived(BMessage *msg)
{
	uint8 key = 0xff;
	uint8 clicks = 0;

	switch(msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
			if(msg->FindInt8("key", (int8*)&key) != B_OK) break;
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(key >= OLED_BUTTONS_NUM || clicks == 0) break;

			fKeyState |= (0x01 << key);
			if(msg->what == B_KEY_DOWN)
			{
				fKeyState |= (0x0100 << key);
				if(fStandingInView != NULL)
					fStandingInView->KeyDown(key, clicks);
				else
					KeyDown(key, clicks);
			}
			else
			{
				fKeyState &= ~(0x0100 << key);
				if(fStandingInView != NULL)
					fStandingInView->KeyUp(key, clicks);
				else
					KeyUp(key, clicks);
				fKeyState &= ~(0x01 << key);
			}
			break;

		case B_PULSE:
			if(fStandingInView != NULL)
				fStandingInView->Pulse();
			else
				Pulse();
			break;

		case '_UPN': // like _UPDATE_IF_NEEDED_ in BeOS API
			if(IsActivated() == false) break;
			if(IsNeededToRegen())
				fUpdateRect = OLEDView::Bounds();
			else
				fUpdateRect &= OLEDView::Bounds();

			if(fUpdateRect.IsValid())
			{
				EnableUpdate(false);
				FillRect(fUpdateRect, B_SOLID_LOW); // auto clear
				if(fStandingInView != NULL)
					fStandingInView->Draw(fUpdateRect);
				else
					Draw(fUpdateRect);
				EnableUpdate(true);

				fUpdateRect = BRect();
			}
			break;

		default:
			BHandler::MessageReceived(msg);
	}
}


void
OLEDView::SetActivated(bool state)
{
	if(fActivated != state)
	{
		fKeyState = 0;
		fActivated = state;
		Activated(state);
	}
}


bool
OLEDView::IsActivated() const
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			return fMasterView->IsActivated();
		return false;
	}

	return fActivated;
}


void
OLEDView::Activated(bool state)
{
	if(fStandingInView != NULL) // for derived class
		fStandingInView->Activated(state);

	if(fMasterView != NULL) return;

	if(state)
	{
		fUpdateRect = BRect();
		fUpdateCount = 0;
		InvalidRect(); // redraw later
	}
}


void
OLEDView::InvalidRect()
{
	InvalidRect(OLEDView::Bounds());
}


void
OLEDView::InvalidRect(BRect r)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->InvalidRect(r);
		return;
	}

	if(Looper() == NULL) return;
	if(r.IsValid() == false) return;

	if(fUpdateRect.IsValid())
		fUpdateRect |= r;
	else
		fUpdateRect = r;
	Looper()->PostMessage('_UPN', this); // like _UPDATE_IF_NEEDED_ in BeOS API
}


OLEDView*
OLEDView::StandingInView() const
{
	return fStandingInView;
}


bool
OLEDView::AddStickView(OLEDView *view)
{
	if(view == NULL) return false;
	if(!(view->Looper() == NULL || view->Looper() == this->Looper())) return false;
	if(view->fMasterView != NULL) return false;

	if(fStickViews.AddItem(view) == false) return false;
	view->fMasterView = this;
	if(Looper() != NULL)
	{
		Looper()->AddHandler(view);
		view->Attached();
	}

	return true;
}


bool
OLEDView::RemoveStickView(OLEDView *view)
{
	if(view == NULL) return false;
	if(view->fMasterView != this) return false;

	if(fStickViews.RemoveItem(view) == false) return false;
	if(Looper() != NULL)
	{
		view->Detached();
		Looper()->RemoveHandler(view);
	}
	view->fMasterView = NULL;

	if(view == fStandingInView)
		fStandingInView = NULL;

	return true;
}


OLEDView*
OLEDView::RemoveStickView(int32 index)
{
	OLEDView *view = (OLEDView*)fStickViews.ItemAt(index);
	if(view == NULL || RemoveStickView(view) == false) return NULL;
	return view;
}


int32
OLEDView::CountStickViews() const
{
	return fStickViews.CountItems();
}


OLEDView*
OLEDView::StickViewAt(int32 index) const
{
	return((OLEDView*)fStickViews.ItemAt(index));
}


OLEDView*
OLEDView::MasterView() const
{
	return fMasterView;
}


OLEDView*
OLEDView::TopView() const
{
	OLEDView *view = fMasterView;

	while(!(view == NULL || view->fMasterView == NULL))
		view = view->fMasterView;

	return view;
}


void
OLEDView::Attached()
{
	if(Looper() == NULL) return;

	for(int32 k = 0; k < fStickViews.CountItems(); k++)
	{
		OLEDView *view = (OLEDView*)fStickViews.ItemAt(k);

		Looper()->AddHandler(view);
		view->Attached();
	}
}


void
OLEDView::Detached()
{
	if(Looper() == NULL) return;

	for(int32 k = 0; k < fStickViews.CountItems(); k++)
	{
		OLEDView *view = (OLEDView*)fStickViews.ItemAt(k);

		view->Detached();
		Looper()->RemoveHandler(view);
	}
}


bool
OLEDView::IsStoodIn() const
{
	return(fStandingInView != NULL);
}


void
OLEDView::StandIn()
{
	if(fMasterView == NULL || fMasterView->fStandingInView == this) return;
	fMasterView->fStandingInView = this;
	fMasterView->InvalidRect();
}


void
OLEDView::StandBack()
{
	if(fMasterView == NULL || fMasterView->fStandingInView != this) return;
	fMasterView->fStandingInView = NULL;
	fMasterView->InvalidRect();
}

