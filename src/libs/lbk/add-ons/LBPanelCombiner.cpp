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

#include <lbk/add-ons/LBPanelCombiner.h>

LBPanelCombiner::LBPanelCombiner()
	: LBPanelDevice(),
	  fCombineStyle(LBK_SCREEN_COMBINE_BY_SINGLE),
	  fWidth(0),
	  fHeight(0),
	  fKeysCount(0),
	  fState(true),
	  fBlockKeyEvents(false),
	  fBlockTimestamp(0)
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
	// TODO: messenger, etc.

	for(int k = 0; k < fScreens.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fScreens.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelScreen *screen = cast_as(item, LBPanelScreen);

		delete screen;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}

	for(int k = 0; k < fKeypads.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fKeypads.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelKeypad *keypad = cast_as(item, LBPanelKeypad);

		delete keypad;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}

	for(int k = 0; k < fTouchpads.CountItems(); k++)
	{
		LBPanelDeviceAddOn *item = (LBPanelDeviceAddOn*)fTouchpads.ItemAt(k);
		void *image = item->fAddOn;
		LBPanelTouchpad *touchpad = cast_as(item, LBPanelTouchpad);

		delete touchpad;
		if(image != NULL) LBPanelDeviceAddOn::UnloadAddOn(image);
	}
}


status_t
LBPanelCombiner::InitCheck(const char *options)
{
	// TODO
	return B_ERROR;
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
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::FillRect(BRect r,
			  pattern p,
			  bool patternVertical,
			  bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


bool
LBPanelCombiner::IsFontHeightSupported(uint8 size)
{
	// TODO
	return false;
}


status_t
LBPanelCombiner::DrawString(const char *str,
			    BPoint pt,
			    uint8 fontHeight,
			    bool erase,
			    bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::MeasureStringWidth(const char *str,
				    uint8 fontHeight,
				    uint16 &width)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::InvertRect(BRect r,
			    bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::GetPowerState(bool &state)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::SetPowerState(bool state, bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::GetTimestamp(bigtime_t &ts)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::SetTimestampNow(bigtime_t &tsRet)
{
	// TODO
	return B_ERROR;
}

status_t
LBPanelCombiner::DisableUpdate()
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::EnableUpdate()
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::GetCountOfKeys(uint8 &count)
{
	// TODO
	return B_ERROR;
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
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::SetLowColor(rgb_color c)
{
	// TODO
	return B_ERROR;
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
	// TODO
	return B_ERROR;
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
		// TODO: block events by SendMessage()
		fBlockKeyEvents = state;

		if(fBlockKeyEvents == false)
			fBlockTimestamp = system_time();
	}

	return B_OK;
}


status_t
LBPanelCombiner::SetCombineStyle(uint32 style)
{
	if(fCombineStyle == style) return B_OK;

	if((style & 0xff) != LBK_SCREEN_COMBINE_BY_SINGLE)
	{
		// TODO
		return B_ERROR;
	}

	// TODO

	fCombineStyle = style;
	return B_OK;
}


status_t
LBPanelCombiner::AddScreen(const char *add_on, BPoint location)
{
	LBPanelScreen* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)func, "instantiate_panel_screen");
	if(image == NULL) return B_ERROR;

	LBPanelScreen *screen = (*func)();
	if(screen == NULL || AddScreen(screen, location) != B_OK)
	{
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
	if(add_on == NULL || fScreens.AddItem(add_on) == false) return B_ERROR;

	if((fCombineStyle & 0x000000ff) == LBK_SCREEN_COMBINE_BY_SINGLE)
	{
		BPoint loc = screen->Location();
		uint16 w = (uint16)(loc.x + (float)screen->Width());
		uint16 h = (uint16)(loc.y + (float)screen->Height());

		fWidth = max_c(fWidth, w);
		fHeight = max_c(fHeight, h);

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

		// TODO: messenger, ID, etc.
	}
	else
	{
		// TODO
	}

	return B_OK;
}


status_t
LBPanelCombiner::AddKeypad(const char *add_on)
{
	LBPanelKeypad* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)func, "instantiate_panel_keypad");
	if(image == NULL) return B_ERROR;

	LBPanelKeypad *keypad = (*func)();
	if(keypad == NULL || AddKeypad(keypad) != B_OK)
	{
		LBPanelDeviceAddOn::UnloadAddOn(image);
		return B_ERROR;
	}

	keypad->LBPanelDeviceAddOn::fAddOn = image;
	return B_OK;
}


status_t
LBPanelCombiner::AddKeypad(LBPanelKeypad *keypad)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::AddTouchpad(const char *add_on)
{
	LBPanelTouchpad* (*func)(void) = NULL;
	void *image = LBPanelDeviceAddOn::LoadAddOn(add_on, (void**)func, "instantiate_panel_touchpad");
	if(image == NULL) return B_ERROR;

	LBPanelTouchpad *touchpad = (*func)();
	if(touchpad == NULL || AddTouchpad(touchpad) != B_OK)
	{
		LBPanelDeviceAddOn::UnloadAddOn(image);
		return B_ERROR;
	}

	touchpad->LBPanelDeviceAddOn::fAddOn = image;
	return B_OK;
}


status_t
LBPanelCombiner::AddTouchpad(LBPanelTouchpad *touchpad)
{
	// TODO
	return B_ERROR;
}


void
LBPanelCombiner::Init(int32 id, const BMessenger &msgr)
{
	// TODO: screens, keypads, touchpads

	// WARNING: DO NOT CHANGE THE SEQUENCE OF FOLLOWING 2 LINES !!!
	LBPanelDeviceAddOn::fMsgr = msgr;
	LBPanelDeviceAddOn::fID = id;
}

