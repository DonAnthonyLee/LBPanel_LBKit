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
	: LBPanelDevice()
{
}


LBPanelCombiner::~LBPanelCombiner()
{
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
	// TODO
	return 0;
}


uint16
LBPanelCombiner::ScreenHeight()
{
	// TODO
	return 0;
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
	// TODO
	return 0;
}


lbk_color_space
LBPanelCombiner::ScreenColorSpace()
{
	// TODO
	return LBK_CS_UNKNOWN;
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
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::GetSideOfKeys(bool &right_or_bottom)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
			      uint16 &offsetRightBottom)
{
	// TODO
	return B_ERROR;
}
#endif


status_t
LBPanelCombiner::BlockKeyEvents(bool state)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::AddScreen(const char *add_on)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::AddScreen(LBPanelScreen *screen)
{
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::AddKeypad(const char *add_on)
{
	// TODO
	return B_ERROR;
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
	// TODO
	return B_ERROR;
}


status_t
LBPanelCombiner::AddTouchpad(LBPanelTouchpad *touchpad)
{
	// TODO
	return B_ERROR;
}

