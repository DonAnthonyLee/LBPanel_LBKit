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

#include <lbk/LBKConfig.h>
#include <lbk/LBApplication.h> // for "LBK_VIEW_STOOD_BACK"
#include <lbk/LBView.h>


LBView::LBView(const char *name)
	: BHandler(name),
	  fDev(NULL), fTimestamp(0),
	  fActivated(false),
	  fFontSize(12),
	  fKeyState(0),
	  fUpdateCount(0),
	  fMasterView(NULL),
	  fStandingInView(NULL)
{
	fUpdateRect = BRect();
	fStandInTimestamp = (bigtime_t)-1;

#ifdef LBK_ENABLE_MORE_FEATURES
	SetHighColor(255, 255, 255); // white
	SetLowColor(0, 0, 0); // black
	SetViewColor(0, 0, 0); // black
#endif
}


LBView::~LBView()
{
	if(Looper() != NULL)
	{
		// good or not ?
		Looper()->RemoveHandler(this);
	}

	LBView *view;
	while((view = (LBView*)fStickViews.RemoveItem((int32)0)) != NULL) delete view; 
}


void
LBView::FillRect(BRect r, pattern p)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->FillRect(r, p);
		return;
	}

	r &= LBView::Bounds();
	if(fDev == NULL || fActivated == false || r.IsValid() == false) return;

	// avoid apart-drawing
#ifdef ETK_MAJOR_VERSION
	r.Floor();
#else
	r.left = floorf(r.left);
	r.right = floorf(r.right);
	r.top = floorf(r.right);
	r.bottom = floorf(r.bottom);
#endif

#ifdef LBK_ENABLE_MORE_FEATURES
	if(fDev->SetHighColor(fColors[0]) != B_OK) return;
	if(fDev->SetLowColor(fColors[1]) != B_OK) return;
#endif
	fDev->FillRect(r, p, false, fTimestamp);
}


void
LBView::StrokeRect(BRect rect, bool erase)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->StrokeRect(rect, erase);
		return;
	}

	BRect r;
	pattern p = (erase ? B_SOLID_LOW : B_SOLID_HIGH);

	if(fDev == NULL || fActivated == false || rect.IsValid() == false) return;

	// avoid apart-drawing
#ifdef ETK_MAJOR_VERSION
	rect.Floor();
#else
	rect.left = floorf(rect.left);
	rect.right = floorf(rect.right);
	rect.top = floorf(rect.right);
	rect.bottom = floorf(rect.bottom);
#endif

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
LBView::InvertRect(BRect r)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->InvertRect(r);
		return;
	}

	r &= LBView::Bounds();
	if(fDev == NULL || fActivated == false || r.IsValid() == false) return;

	// avoid apart-drawing
#ifdef ETK_MAJOR_VERSION
	r.Floor();
#else
	r.left = floorf(r.left);
	r.right = floorf(r.right);
	r.top = floorf(r.right);
	r.bottom = floorf(r.bottom);
#endif

	fDev->InvertRect(r, fTimestamp);
}


void
LBView::DrawString(const char *str, BPoint pt, bool erase)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->DrawString(str, pt, erase);
		return;
	}

	if(fDev == NULL || fActivated == false) return;

#ifdef LBK_ENABLE_MORE_FEATURES
	if(fDev->SetHighColor(fColors[0]) != B_OK) return;
	if(fDev->SetLowColor(fColors[1]) != B_OK) return;
#endif
	fDev->DrawString(str, pt, fFontSize, erase, fTimestamp);
}


void
LBView::DrawIcon(const lbk_icon *icon, BPoint pt)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->DrawIcon(icon, pt);
		return;
	}

	if(fDev == NULL || fActivated == false) return;
	if(icon == NULL || icon->type > 2 /*LBK_ICON_32x32*/) return;

#ifdef LBK_ENABLE_MORE_FEATURES
	if(fDev->SetHighColor(fColors[0]) != B_OK) return;
	if(fDev->SetLowColor(fColors[1]) != B_OK) return;
#endif

	BRect r;
	pattern p;
	for(int k = 0; k < (1 << icon->type); k++) // y
	{
		for(int m = 0; m < (1 << icon->type); m++) // x
		{
			r.left = pt.x + (m << 3);
			r.top = pt.y + (k << 3);
			r.right = r.left + 7;
			r.bottom = r.top + 7;

			memcpy(p.data,
			       icon->data + ((k * (1 << icon->type) + m) << 3),
			       sizeof(p.data));

			fDev->FillRect(r, p, true, fTimestamp);
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
	if(fMasterView == NULL || fMasterView->fStandingInView != this)
		return fFontSize;
	return(fMasterView->FontSize());
}


void
LBView::SetFontSize(uint8 size)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->SetFontSize(size);
		return;
	}

	if(fDev == NULL) return;
	if(fDev->IsFontHeightSupported(size)) fFontSize = size;
}


uint8
LBView::FittingFontSize(BRect r) const
{
	if(fMasterView != NULL)
		return fMasterView->FittingFontSize(r);

	if(fDev == NULL || r.Height() < 10) return 0;

	uint8 height = (uint8)(r.Height() > 34 ? 32 : (r.Height() - 2));

	while(height > 6 && fDev->IsFontHeightSupported(height) == false) height >>= 1;
	return(height == 6 ? 0 : height);
}


uint16
LBView::StringWidth(const char *str) const
{
	if(fMasterView != NULL)
	{
		return(fMasterView->fStandingInView == this ?
				fMasterView->StringWidth(str) : 0);
	}

	if(fDev == NULL || str == NULL || *str == 0) return 0;

	uint16 w = 0;
	fDev->MeasureStringWidth(str, fFontSize, w);
	return w;
}


void
LBView::EnableUpdate(bool state)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->EnableUpdate(state);
		return;
	}

	if(fDev == NULL || fActivated == false) return;

	if(state == false)
	{
		if(fUpdateCount == 0)
			if(fDev->DisableUpdate() != B_OK) return;

		fUpdateCount++;
	}
	else if(fUpdateCount > 0)
	{
		if(fUpdateCount == 1)
			if(fDev->EnableUpdate() != B_OK) return;

		fUpdateCount--;
	}
}


bool
LBView::IsNeededToRegen() const
{
	if(fMasterView != NULL)
	{
		return(fMasterView->fStandingInView == this ?
				fMasterView->IsNeededToRegen() : false);
	}

	if(fDev == NULL || fActivated == false) return false;

	bigtime_t ts = 0;
	fDev->GetTimestamp(ts);
	return(ts > fTimestamp);
}


bool
LBView::GetPowerState() const
{
	if(fMasterView != NULL)
		return fMasterView->GetPowerState();

	if(fDev == NULL) return false;

	bool state = false;
	fDev->GetPowerState(state);
	return state;
}


void
LBView::SetPowerState(bool state)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->fStandingInView == this)
			fMasterView->SetPowerState(state);
		return;
	}

	if(fDev == NULL || fActivated == false) return;
	fDev->SetPowerState(state, fTimestamp);
}


BRect
LBView::Bounds() const
{
	BRect r(0, 0, 0, 0);

	LBPanelDevice *dev = PanelDevice();
	if(dev != NULL)
	{
		r.right = (float)dev->ScreenWidth() - 1;
		r.bottom = (float)dev->ScreenHeight() - 1;
	}

	return r;
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
			if(clicks == 0) break;
#if 0 // don't care
			if(key >= CountPanelKeys()) break;
#else
			if(key > LBK_KEY_ID_MAX) break;
#endif

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
				Clear(fUpdateRect); // auto clear
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
		Invalidate(); // redraw later
	}
}


void
LBView::Invalidate()
{
	Invalidate(LBView::Bounds());
}


void
LBView::Invalidate(BRect r)
{
	if(fMasterView != NULL)
	{
		if(fMasterView->IsStoodIn())
			fMasterView->Invalidate(r);
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
	fMasterView->Invalidate();
	fMasterView->fKeyState = 0; // reset the key state of master view too !
	fKeyState = 0;
	fStandInTimestamp = real_time_clock_usecs();
}


void
LBView::StandBack()
{
	if(fMasterView == NULL || fMasterView->fStandingInView != this) return;
	fMasterView->fStandingInView = NULL;
	fMasterView->Invalidate();
	fStandInTimestamp = (bigtime_t)-1;

	if(Looper() != NULL)
	{
		BMessage msg(LBK_VIEW_STOOD_BACK);
		msg.AddPointer("view", reinterpret_cast<void*>(this));

		Looper()->PostMessage(&msg, fMasterView);
	}
}


bigtime_t
LBView::GetStandInTime() const
{
	if(fStandInTimestamp < (bigtime_t)0) return -1;
	return(real_time_clock_usecs() - fStandInTimestamp);
}


LBPanelDevice*
LBView::PanelDevice() const
{
	if(fMasterView != NULL)
		return(fMasterView->fStandingInView == this ? fMasterView->PanelDevice() : NULL);
	return fDev;
}


uint8
LBView::CountPanelKeys() const
{
	uint8 n = 0;

	if(fMasterView != NULL)
		return fMasterView->CountPanelKeys();

	if(fDev != NULL)
		fDev->GetCountOfKeys(n);

	return n;
}


#ifdef LBK_ENABLE_MORE_FEATURES
void
LBView::SetHighColor(rgb_color c)
{
	*((uint32*)&fColors[0]) = *((uint32*)&c);
}


void
LBView::SetHighColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	fColors[0].red = r;
	fColors[0].green = g;
	fColors[0].blue = b;
	fColors[0].alpha = a;
}


void
LBView::SetLowColor(rgb_color c)
{
	*((uint32*)&fColors[1]) = *((uint32*)&c);
}


void
LBView::SetLowColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	fColors[1].red = r;
	fColors[1].green = g;
	fColors[1].blue = b;
	fColors[1].alpha = a;
}


void
LBView::SetViewColor(rgb_color c)
{
	*((uint32*)&fColors[2]) = *((uint32*)&c);
}


void
LBView::SetViewColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	fColors[2].red = r;
	fColors[2].green = g;
	fColors[2].blue = b;
	fColors[2].alpha = a;
}


rgb_color
LBView::HighColor() const
{
	return fColors[0];
}


rgb_color
LBView::LowColor() const
{
	return fColors[1];
}


rgb_color
LBView::ViewColor() const
{
	return fColors[2];
}
#endif


void
LBView::Clear(BRect r)
{
#ifdef LBK_ENABLE_MORE_FEATURES
	rgb_color c = LowColor();
	SetLowColor(ViewColor());
	FillRect(r, B_SOLID_LOW);
	SetLowColor(c);
#else
	FillRect(r, B_SOLID_LOW);
#endif
}

