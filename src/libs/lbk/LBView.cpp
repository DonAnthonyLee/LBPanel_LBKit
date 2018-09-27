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
 * File: LBView.cpp
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
#include <lbk/LBView.h>

LBView::LBView(const char *name)
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
	fStandInTimestamp = (bigtime_t)-1;
}


LBView::~LBView()
{
	LBView *view;
	while((view = (LBView*)fStickViews.RemoveItem((int32)0)) != NULL) delete view; 
}


void
LBView::FillRect(BRect r, pattern p)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->FillRect(r, p);
		return;
	}

	_lbk_ssd1306_clear_t data;

	r &= LBView::Bounds();
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

	if(ioctl(fFD, LBK_SSD1306_IOC_CLEAR, &data) == 0) fTimestamp = data.ts;
}


void
LBView::StrokeRect(BRect rect, bool erase)
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
LBView::DrawString(const char *str, BPoint pt, bool erase)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->DrawString(str, pt, erase);
		return;
	}

	_lbk_ssd1306_show_t data;

	if(str == NULL || *str == 0) return;
	if(fFD < 0 || fActivated == false) return;
	if(pt.x < 0 || pt.y < 0) return;

	bzero(&data, sizeof(data));
	data.x = (uint8_t)pt.x;
	data.y = (uint8_t)pt.y;
	data.size = fFontSize;
	strncpy(data.str, str, sizeof(data.str));
	data.erase_mode = erase ? 1 : 0;

	if(ioctl(fFD, LBK_SSD1306_IOC_SHOW, &data) == 0) fTimestamp = data.ts;
}


void
LBView::DrawIcon(const lbk_icon *icon, BPoint pt)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->DrawIcon(icon, pt);
		return;
	}

	_lbk_ssd1306_clear_t data;

	if(fFD < 0 || fActivated == false) return;
	if(icon == NULL || icon->type > 2 /*LBK_ICON_32x32*/) return;
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

			if(ioctl(fFD, LBK_SSD1306_IOC_CLEAR, &data) == 0) fTimestamp = data.ts;
		}
	}
}


void
LBView::DrawIcon(lbk_icon_id id, BPoint pt)
{
	DrawIcon(lbk_get_icon_data(id), pt);
}


uint8
LBView::FontSize() const
{
	if(fMasterView != NULL)
		return fMasterView->FontSize();

	return fFontSize;
}


void
LBView::SetFontSize(uint8 size)
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
LBView::StringWidth(const char *str) const
{
	if(fMasterView != NULL)
		return fMasterView->StringWidth(str);

	_lbk_ssd1306_string_width_t data;

	if (fFD < 0 || str == NULL || *str == 0) return 0;

	bzero(&data, sizeof(data));
	data.w = data.h = 0;
	data.size = fFontSize;
	strncpy(data.str, str, sizeof(data.str));

	return((ioctl(fFD, LBK_SSD1306_IOC_STRING_WIDTH, &data) == 0) ? data.w : 0);
}


void
LBView::EnableUpdate(bool state)
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
			if(ioctl(fFD, LBK_SSD1306_IOC_UPDATE, &st) != 0) return;

		fUpdateCount++;
	}
	else if(fUpdateCount > 0)
	{
		if(fUpdateCount == 1)
			if(ioctl(fFD, LBK_SSD1306_IOC_UPDATE, &st) != 0) return;

		fUpdateCount--;
	}
}


bool
LBView::IsNeededToRegen() const
{
	if(fMasterView != NULL)
		return fMasterView->IsNeededToRegen();

	_lbk_ssd1306_get_ts_t data;

	if(fFD < 0 || fActivated == false) return false;

	data.last_action = 0;
	if(ioctl(fFD, LBK_SSD1306_IOC_TIMESTAMP, &data) != 0) return false;

	return(data.ts > fTimestamp);
}


bool
LBView::GetPowerState() const
{
	if(fMasterView != NULL)
		return fMasterView->GetPowerState();

	_lbk_ssd1306_power_t data;

	if (fFD < 0) return false;

	data.state = 2;
	if(ioctl(fFD, LBK_SSD1306_IOC_POWER, &data) != 0) return false;
	return(data.state == 1);
}


void
LBView::SetPowerState(bool state)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->SetPowerState(state);
		return;
	}

	_lbk_ssd1306_power_t data;

	if (fFD < 0 || fActivated == false) return;

	data.state = (state ? 1 : 0);
	if(ioctl(fFD, LBK_SSD1306_IOC_POWER, &data) == 0) fTimestamp = data.ts;
}


BRect
LBView::Bounds() const
{
	return BRect(0, 0, OLED_SCREEN_WIDTH - 1, OLED_SCREEN_HEIGHT - 1);
}


void
LBView::Draw(BRect updateRect)
{
	// Empty
}


void
LBView::KeyDown(uint8 key, uint8 clicks)
{
	// Empty
}


void
LBView::KeyUp(uint8 key, uint8 clicks)
{
	// Empty
}


void
LBView::Pulse()
{
	// Empty
}


uint8
LBView::KeyState(uint8 *down_state) const
{
	if(down_state) *down_state = (fKeyState >> 8);
	return fKeyState;
}

void
LBView::MessageReceived(BMessage *msg)
{
	uint8 key = 0xff;
	uint8 clicks = 0;

	switch(msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
			if(fStandingInView != NULL)
			{
				// let the view handle the message itself
				Looper()->PostMessage(msg, fStandingInView);
				break;
			}
			if(msg->FindInt8("key", (int8*)&key) != B_OK) break;
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(key >= OLED_BUTTONS_NUM || clicks == 0) break;

			fKeyState |= (0x01 << key);
			if(msg->what == B_KEY_DOWN)
			{
				fKeyState |= (0x0100 << key);
				if(GetPowerState() == false) break;
				KeyDown(key, clicks);
			}
			else if((fKeyState & (0x0100 << key)) != 0) // must be DOWN before
			{
				fKeyState &= ~(0x0100 << key);
				if(GetPowerState() == false)
					SetPowerState(true);
				else
					KeyUp(key, clicks);
				fKeyState &= ~(0x01 << key);
			}
			break;

		case B_PULSE:
			if(fStandingInView != NULL)
			{
				// let the view handle the message itself
				Looper()->PostMessage(msg, fStandingInView);
				break;
			}
			Pulse();
			break;

		case '_UPN': // like _UPDATE_IF_NEEDED_ in BeOS API
			if(IsActivated() == false) break;
			if(IsNeededToRegen())
				fUpdateRect = LBView::Bounds();
			else
				fUpdateRect &= LBView::Bounds();

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
LBView::SetActivated(bool state)
{
	if(fActivated != state)
	{
		fKeyState = 0;
		fActivated = state;
		this->Activated(state);
	}
}


bool
LBView::IsActivated() const
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
LBView::Activated(bool state)
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
LBView::InvalidRect()
{
	InvalidRect(LBView::Bounds());
}


void
LBView::InvalidRect(BRect r)
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


LBView*
LBView::StandingInView() const
{
	return fStandingInView;
}


bool
LBView::AddStickView(LBView *view)
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
LBView::RemoveStickView(LBView *view)
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


LBView*
LBView::RemoveStickView(int32 index)
{
	LBView *view = (LBView*)fStickViews.ItemAt(index);
	if(view == NULL || RemoveStickView(view) == false) return NULL;
	return view;
}


int32
LBView::CountStickViews() const
{
	return fStickViews.CountItems();
}


LBView*
LBView::StickViewAt(int32 index) const
{
	return((LBView*)fStickViews.ItemAt(index));
}


LBView*
LBView::FindStickView(const char *name) const
{
	for(int32 k = 0; k < fStickViews.CountItems(); k++)
	{
		LBView *view = (LBView*)fStickViews.ItemAt(k);

		if(name == NULL)
		{
			if(view->Name() == NULL) return view;
			continue;
		}
		if(view->Name() == NULL) continue;
		if(strcmp(name, view->Name()) == 0) return view;
	}

	return NULL;
}


LBView*
LBView::MasterView() const
{
	return fMasterView;
}


LBView*
LBView::TopView() const
{
	LBView *view = fMasterView;

	while(!(view == NULL || view->fMasterView == NULL))
		view = view->fMasterView;

	return view;
}


void
LBView::Attached()
{
	if(Looper() == NULL) return;

	for(int32 k = 0; k < fStickViews.CountItems(); k++)
	{
		LBView *view = (LBView*)fStickViews.ItemAt(k);

		Looper()->AddHandler(view);
		view->Attached();
	}
}


void
LBView::Detached()
{
	if(Looper() == NULL) return;

	for(int32 k = 0; k < fStickViews.CountItems(); k++)
	{
		LBView *view = (LBView*)fStickViews.ItemAt(k);

		view->Detached();
		Looper()->RemoveHandler(view);
	}
}


bool
LBView::IsStoodIn() const
{
	return(fStandingInView != NULL);
}


void
LBView::StandIn()
{
	if(fMasterView == NULL || fMasterView->fStandingInView == this) return;
	fMasterView->fStandingInView = this;
	fMasterView->InvalidRect();
	fKeyState = 0;
	fStandInTimestamp = real_time_clock_usecs();
}


void
LBView::StandBack()
{
	if(fMasterView == NULL || fMasterView->fStandingInView != this) return;
	fMasterView->fStandingInView = NULL;
	fMasterView->InvalidRect();
	fStandInTimestamp = (bigtime_t)-1;
}


bigtime_t
LBView::GetStandInTime() const
{
	if(fStandInTimestamp < (bigtime_t)0) return -1;
	return(real_time_clock_usecs() - fStandInTimestamp);
}

