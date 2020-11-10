/* --------------------------------------------------------------------------
 *
 * Little Board Application Kit
 * Copyright (C) 2018-2019, Anthony Lee, All Rights Reserved
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
 * File: LBPanelCombiner.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdlib.h>
#include <lbk/add-ons/LBPanelCombiner.h>


LBPanelCombiner::LBPanelCombiner()
	: LBPanelDevice(),
	  fCombineStyle(LBK_SCREEN_COMBINE_BY_SINGLE),
	  fWidth(0),
	  fHeight(0),
	  fBlockKeyEvents(false),
	  fBlockTimestamp(0),
	  fAllowNoPad(false)
{
#ifdef LBK_ENABLE_MORE_FEATURES
	fDepth = 0;
	fColorSpace = LBK_CS_UNKNOWN;
	fOrientation = B_HORIZONTAL;
	fKeysRB = true;
	fKeysOffset[0] = fKeysOffset[1] = 0;
#endif
}


LBPanelCombiner::~LBPanelCombiner()
{
	// TODO

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		item->fDev = NULL;
		delete screen;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}

	for(int32 k = 0; k < fKeypads.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fKeypads.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelKeypad *keypad = cast_as(item, LBPanelKeypad);

		item->fDev = NULL;
		delete keypad;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}

	for(int32 k = 0; k < fTouchpads.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fTouchpads.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelTouchpad *touchpad = cast_as(item, LBPanelTouchpad);

		item->fDev = NULL;
		delete touchpad;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}
}


status_t
LBPanelCombiner::InitCheck(const char *options)
{
	BString opt(options);

	opt.ReplaceAll(",", " ");
	while(opt.FindFirst("  ") >= 0) opt.ReplaceAll("  ", " ");
	while(opt.Length() > 0)
	{
		int32 found = opt.FindFirst(' ');
		if(found < 0) found = opt.Length();

		BString item(opt.String(), found);
		opt.Remove(0, found + 1);

		if(item.Length() <= 0) continue;

		BString value;
		found = item.FindFirst('=');
		if(found >= 0)
		{
			value.SetTo(item.String() + found + 1);
			item.Truncate(found);
		}

		// TODO

		if(item == "screen")
		{
			// seemed like: screen=/usr/lib/add-ons/lbk/oled-ssd1306.so:0:0:dev=/dev/oled-003c
			BPoint loc(B_ORIGIN);
			BString optScreen;
			found = value.FindFirst(':');
			if(found > 0)
			{
				optScreen.SetTo(value.String() + found + 1);
				value.Truncate(found);

				int32 last_found = optScreen.FindLast(':');
				found = optScreen.FindFirst(':');

				if(found > 0 && found != last_found)
				{
					loc.x = (float)atoi(optScreen.String());
					loc.y = (float)atoi(optScreen.String() + found + 1);
					if(last_found < 0)
						optScreen.Truncate(0);
					else
						optScreen.Remove(0, last_found + 1);
				}
			}

			AddScreen(value.String(), loc, optScreen.String());
		}
		else if(item == "keypad")
		{
			// seemed like: keypad=/usr/lib/add-ons/lbk/keypad-generic.so:dev=/dev/input/event0:105:102:106
			BString optKeypad;
			found = value.FindFirst(':');
			if(found > 0)
			{
				optKeypad.SetTo(value.String() + found + 1);
				value.Truncate(found);
			}

			AddKeypad(value.String(), optKeypad.String());
		}
		else if(item == "touchpad")
		{
			// seemed like: touchpad=/usr/lib/add-ons/lbk/touchpad-generic.so:dev=/dev/input/event1
			BString optTouchpad;
			found = value.FindFirst(':');
			if(found > 0)
			{
				optTouchpad.SetTo(value.String() + found + 1);
				value.Truncate(found);
			}

			AddTouchpad(value.String(), optTouchpad.String());
		}
		else if(item == "allow_no_pad")
		{
			// seemed like: allow_no_pad=1
			fAllowNoPad = (value.ICompare("true") == 0 || value == "1");
		}
#ifdef LBK_ENABLE_MORE_FEATURES
		else if(item == "keys_RB")
		{
			fKeysRB = (value == "1" || value.ICompare("true") == 0);
		}
		else if(item == "orient")
		{
			fOrientation = (value == "0" || value.ICompare("horizontal") == 0) ? B_HORIZONTAL : B_VERTICAL;
		}
		else if(item == "offset_LT")
		{
			int v = atoi(value.String());
			if(v >= 0 && v < 65535) fKeysOffset[0] = (uint16)v;
		}
		else if(item == "offset_RB")
		{
			int v = atoi(value.String());
			if(v >= 0 && v < 65535) fKeysOffset[1] = (uint16)v;
		}
#endif
	}

	if(fScreens.CountItems() == 0) return B_ERROR;

	uint8 keys_count = 0;
	GetCountOfKeys(keys_count);

	return((fAllowNoPad || (fTouchpads.CountItems() > 0 || keys_count > 0)) ? B_OK : B_ERROR);
}


uint16
LBPanelCombiner::ScreenWidth()
{
	return fWidth;
}


uint16
LBPanelCombiner::ScreenHeight()
{
	return fHeight;
}


status_t
LBPanelCombiner::ConstrainClipping(BRect r, bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	ts = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		BRect rScr;
		if(r.IsValid())
		{
			rScr.SetLeftTop(screen->fLocation);
			rScr.SetRightBottom(rScr.LeftTop() + BPoint(screen->Width(), screen->Height()) - BPoint(1, 1));
			if(rScr.Intersects(r) == false) continue;

			rScr &= r;
			rScr.OffsetBy(B_ORIGIN - screen->fLocation);
		}

		if(screen->ConstrainClipping(rScr, t) == B_OK)
			ts = max_c(t, ts);
	}

	fClipping = r;

	return B_OK;
}


status_t
LBPanelCombiner::FillRect(BRect r,
			  pattern p,
			  bool patternVertical,
			  bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	ts = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		BRect rScr(screen->fLocation,
			   screen->fLocation + BPoint(screen->Width(), screen->Height()) - BPoint(1, 1));
		if(fClipping.IsValid())
			rScr &= fClipping;
		if(rScr.Intersects(r) == false) continue;

		rScr &= r;
		rScr.OffsetBy(B_ORIGIN - screen->fLocation);

		if(screen->FillRect(rScr, p, patternVertical, t) == B_OK)
			ts = max_c(t, ts);
	}

	return B_OK;
}


bool
LBPanelCombiner::IsFontHeightSupported(uint8 size)
{
	bool retVal;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return false;

	if(fScreens.CountItems() == 1)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(0);

		retVal = cast_as(item, LBPanelScreen)->IsFontHeightSupported(size);
	}
	else
	{
		// TODO
		retVal = false;
	}

	return retVal;
}


status_t
LBPanelCombiner::DrawString(const char *str,
			    BPoint pt,
			    uint8 fontHeight,
			    bool erase,
			    bigtime_t &ts)
{
	status_t retVal;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	if(fScreens.CountItems() == 1)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(0);
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		retVal = screen->DrawString(str, pt + screen->fLocation, fontHeight, erase, ts);
	}
	else
	{
		// TODO
		retVal = B_ERROR;
	}

	return retVal;
}


status_t
LBPanelCombiner::MeasureStringWidth(const char *str,
				    uint8 fontHeight,
				    uint16 &width)
{
	status_t retVal;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	if(fScreens.CountItems() == 1)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(0);

		retVal = cast_as(item, LBPanelScreen)->MeasureStringWidth(str, fontHeight, width);
	}
	else
	{
		// TODO
		retVal = B_ERROR;
	}

	return retVal;
}


status_t
LBPanelCombiner::InvertRect(BRect r,
			    bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	ts = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		BRect rScr(screen->fLocation,
			   screen->fLocation + BPoint(screen->Width(), screen->Height()) - BPoint(1, 1));
		if(fClipping.IsValid())
			rScr &= fClipping;
		if(rScr.Intersects(r) == false) continue;

		rScr &= r;
		rScr.OffsetBy(B_ORIGIN - screen->fLocation);

		if(screen->InvertRect(rScr, t) == B_OK)
			ts = max_c(t, ts);
	}

	return B_OK;
}


status_t
LBPanelCombiner::GetPowerState(bool &state)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	state = true;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		if(cast_as(item, LBPanelScreen)->GetPowerState(state) != B_OK) continue;
		if(state == false) break;
	}

	return B_OK;
}


status_t
LBPanelCombiner::SetPowerState(bool state, bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	ts = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		if(cast_as(item, LBPanelScreen)->SetPowerState(state, t) == B_OK)
			ts = max_c(t, ts);
	}

	return B_OK;
}


status_t
LBPanelCombiner::GetTimestamp(bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	ts = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		if(cast_as(item, LBPanelScreen)->GetTimestamp(t) == B_OK)
			ts = max_c(t, ts);
	}

	return B_OK;
}


status_t
LBPanelCombiner::SetTimestampNow(bigtime_t &tsRet)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	bigtime_t t = 0;
	tsRet = 0;
	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		if(cast_as(item, LBPanelScreen)->SetTimestampNow(t) == B_OK)
			tsRet = max_c(t, tsRet);
	}

	return B_OK;
}


status_t
LBPanelCombiner::DisableUpdate()
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		// ignore return value
		cast_as(item, LBPanelScreen)->DisableUpdate();
	}

	return B_OK;
}


status_t
LBPanelCombiner::EnableUpdate()
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		// ignore return value
		cast_as(item, LBPanelScreen)->EnableUpdate();
	}

	return B_OK;
}


status_t
LBPanelCombiner::GetCountOfKeys(uint8 &count)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fKeypads.CountItems() == 0) return B_ERROR;

	uint8 t;
	count = 0;
	for(int32 k = 0; k < fKeypads.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fKeypads.ItemAt(k);

		if(cast_as(item, LBPanelKeypad)->GetCountOfKeys(t) != B_OK) continue;
		if(0xff - count < t || count + t >= LBK_KEY_MAXIMUM_NUMBER)
		{
			count = LBK_KEY_MAXIMUM_NUMBER;
			break;
		}
		count += t;
	}

	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
uint8
LBPanelCombiner::ScreenDepth()
{
	return fDepth;
}


lbk_color_space
LBPanelCombiner::ScreenColorSpace()
{
	return fColorSpace;
}


status_t
LBPanelCombiner::SetHighColor(rgb_color c)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		// ignore return value
		cast_as(item, LBPanelScreen)->SetHighColor(c);
	}

	return B_OK;
}


status_t
LBPanelCombiner::SetLowColor(rgb_color c)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		// ignore return value
		cast_as(item, LBPanelScreen)->SetLowColor(c);
	}

	return B_OK;
}
#endif


status_t
LBPanelCombiner::MapBuffer(void **buf, size_t *len)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::UnmapBuffer()
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::Flush(bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


void
LBPanelCombiner::Sync()
{
	// TODO
}


status_t
LBPanelCombiner::SetPowerOffTimeout(bigtime_t t)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.CountItems() == 0) return B_ERROR;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);

		// ignore return value
		cast_as(item, LBPanelScreen)->SetPowerOffTimeout(t);
	}

	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
status_t
LBPanelCombiner::GetOrientationOfKeys(orientation &o)
{
	o = fOrientation;

	return B_OK;
}


status_t
LBPanelCombiner::GetSideOfKeys(bool &right_or_bottom)
{
	right_or_bottom = fKeysRB;

	return B_OK;
}


status_t
LBPanelCombiner::GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
				       uint16 &offsetRightBottom)
{
	offsetLeftTop = fKeysOffset[0];
	offsetRightBottom = fKeysOffset[1];

	if(offsetLeftTop > ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight))
		offsetLeftTop = ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight);

	if(offsetRightBottom > ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight) - offsetLeftTop)
		offsetRightBottom = ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight) - offsetLeftTop;

	return B_OK;
}
#endif


status_t
LBPanelCombiner::BlockKeyEvents(bool state)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	if(fBlockKeyEvents != state)
	{
		fBlockKeyEvents = state;

		if(fBlockKeyEvents == false)
			fBlockTimestamp = system_time();
	}

	return B_OK;
}


status_t
LBPanelCombiner::SetCombineStyle(uint32 style)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	if(fCombineStyle != style)
	{
		if((style & 0x000000ff) != LBK_SCREEN_COMBINE_BY_SINGLE)
		{
			// TODO
			return B_ERROR;
		}

		// TODO

		fCombineStyle = style;
	}

	return B_OK;
}


status_t
LBPanelCombiner::AddScreen(const char *add_on, BPoint location, const char *options)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	LBPanelScreen* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)&func, "instantiate_panel_screen");
	if(image == NULL) return B_ERROR;

	LBPanelScreen *screen = (*func)();
	if(screen == NULL || screen->InitCheck(options) != B_OK || AddScreen(screen, location) != B_OK)
	{
		if(screen != NULL) delete screen;
		LBPanelDeviceAddOn::UnloadAddOn(image);
		return B_ERROR;
	}

	screen->LBPanelDeviceAddOn::fAddOn = image;
	return B_OK;
}


status_t
LBPanelCombiner::AddScreen(LBPanelScreen *screen, BPoint location)
{
	LBPanelDeviceAddOn *add_on = e_cast_as(screen, LBPanelDeviceAddOn);
	if(add_on == NULL || add_on->fDev != NULL) return B_BAD_VALUE;

	BRect r((int16)0x8000, (int16)0x8000, (int16)0x7fff, (int16)0x7fff);
	BRect rScr(location, location + BPoint(screen->Width(), screen->Height()) - BPoint(1, 1));
	if(rScr.right < 0 || rScr.bottom < 0 ||
	   r.Contains(location) == false ||
	   r.Contains(rScr) == false) return B_BAD_VALUE;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fScreens.AddItem(add_on) == false) return B_ERROR;

	if((fCombineStyle & 0x000000ff) == LBK_SCREEN_COMBINE_BY_SINGLE)
	{
		fWidth = max_c(fWidth, ((uint16)(rScr.right + 1)));
		fHeight = max_c(fHeight, ((uint16)(rScr.bottom + 1)));

#ifdef LBK_ENABLE_MORE_FEATURES
		if(fScreens.CountItems() == 1)
		{
			fDepth = screen->Depth();
			fColorSpace = screen->ColorSpace();
		}
		else if(fDepth != screen->Depth() || fColorSpace != screen->ColorSpace())
		{
			fDepth = 32;
			fColorSpace = LBK_CS_RGB32;
		}
#endif
	}
	else
	{
		// TODO
		return B_ERROR;
	}

	add_on->fDev = this;
	if(LBPanelDeviceAddOn::fID >= 0)
		add_on->fID = fScreens.CountItems() - 1;

	return B_OK;
}


status_t
LBPanelCombiner::AddKeypad(const char *add_on, const char *options)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	LBPanelKeypad* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)&func, "instantiate_panel_keypad");
	if(image == NULL) return B_ERROR;

	LBPanelKeypad *keypad = (*func)();
	if(keypad == NULL || keypad->InitCheck(options) != B_OK || AddKeypad(keypad) != B_OK)
	{
		if(keypad != NULL) delete keypad;
		LBPanelDeviceAddOn::UnloadAddOn(image);
		return B_ERROR;
	}

	keypad->LBPanelDeviceAddOn::fAddOn = image;
	return B_OK;
}


status_t
LBPanelCombiner::AddKeypad(LBPanelKeypad *keypad)
{
	LBPanelDeviceAddOn *add_on = e_cast_as(keypad, LBPanelDeviceAddOn);
	if(add_on == NULL || add_on->fDev != NULL) return B_BAD_VALUE;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fKeypads.AddItem(add_on) == false) return B_ERROR;

	// TODO

	add_on->fDev = this;
	if(LBPanelDeviceAddOn::fID >= 0)
		add_on->fID = fKeypads.CountItems() - 1;

	return B_OK;
}


status_t
LBPanelCombiner::AddTouchpad(const char *add_on, const char *options)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	LBPanelTouchpad* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)&func, "instantiate_panel_touchpad");
	if(image == NULL) return B_ERROR;

	LBPanelTouchpad *touchpad = (*func)();
	if(touchpad == NULL || touchpad->InitCheck(options) != B_OK || AddTouchpad(touchpad) != B_OK)
	{
		if(touchpad != NULL) delete touchpad;
		LBPanelDeviceAddOn::UnloadAddOn(image);
		return B_ERROR;
	}

	touchpad->LBPanelDeviceAddOn::fAddOn = image;
	return B_OK;
}


status_t
LBPanelCombiner::AddTouchpad(LBPanelTouchpad *touchpad)
{
	LBPanelDeviceAddOn *add_on = e_cast_as(touchpad, LBPanelDeviceAddOn);
	if(add_on == NULL || add_on->fDev != NULL) return B_BAD_VALUE;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelCombiner>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fTouchpads.AddItem(add_on) == false) return B_ERROR;

	// TODO

	add_on->fDev = this;
	if(LBPanelDeviceAddOn::fID >= 0)
		add_on->fID = fTouchpads.CountItems() - 1;

	return B_OK;
}


void
LBPanelCombiner::Init(int32 id, const BMessenger &msgr)
{
	LBPanelDeviceAddOn *item;

	for(int32 k = 0; k < fScreens.CountItems(); k++)
	{
		item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		item->fID = k;
	}

	for(int32 k = 0; k < fKeypads.CountItems(); k++)
	{
		item = (LBPanelDeviceAddOn*)fKeypads.ItemAt(k);
		item->fID = k;
	}

	for(int32 k = 0; k < fTouchpads.CountItems(); k++)
	{
		item = (LBPanelDeviceAddOn*)fTouchpads.ItemAt(k);
		item->fID = k;
	}

	// WARNING: DO NOT CHANGE THE SEQUENCE OF FOLLOWING 2 LINES !!!
	LBPanelDeviceAddOn::fMsgr = msgr;
	LBPanelDeviceAddOn::fID = id;
}


status_t
LBPanelCombiner::SendMessage(const BMessage *msg)
{
	int32 id;
	bigtime_t when;
	uint8 key;

	switch(msg->what)
	{
		case LBK_DEVICE_DETACHED: // device detached when error occurred
			// TODO
			break;

		case B_KEY_DOWN:
		case B_KEY_UP:
			if(msg->FindInt32("keypad_id", &id) != B_OK || id < 0) break;
			if(msg->FindInt64("when", &when) != B_OK) break;
			if(msg->HasInt8("key") == false && msg->HasInt16("key"))
				return LBPanelDevice::SendMessage(msg);
			if(msg->FindInt8("key", (int8*)&key) != B_OK || key > LBK_KEY_ID_MAX) break;

			if(Lock() == false) break;

			if(id >= fKeypads.CountItems() ||
			   (fBlockKeyEvents && fBlockTimestamp > when))
			{
				Unlock();
				break;
			}

			for(int32 k = 0; k < id; k++)
			{
				uint8 t;
				LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fKeypads.ItemAt(k);

				if(cast_as(item, LBPanelKeypad)->GetCountOfKeys(t) != B_OK) continue;
				if(0xff - key < t || key + t > LBK_KEY_ID_MAX)
				{
					key = 0xff;
					break;
				}
				key += t;
			}

			Unlock();

			if(key <= LBK_KEY_ID_MAX)
			{
				BMessage aMsg(*msg);
				aMsg.ReplaceInt8("key", 0, *((int8*)&key));
				return LBPanelDevice::SendMessage(&aMsg);
			}
			break;

		default:
			return LBPanelDevice::SendMessage(msg);
	}

	return B_ERROR;
}

