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
 * File: LBPanelScreen.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_SCREEN_H__
#define __LBK_PANEL_DEVICE_SCREEN_H__

#include <lbk/add-ons/LBPanelDeviceAddOn.h>
#include <lbk/add-ons/LBPanelDevice.h>

#ifdef __cplusplus /* Just for C++ */

// NOTE:
// 	Addon must have C function like below and all the null virtual functions
// 	of class must be implemented.
// 		extern "C" _EXPORT LBPanelScreen* instantiate_panel_screen();


class _EXPORT LBPanelScreen : public LBPanelDeviceAddOn {
public:
	LBPanelScreen();
	virtual ~LBPanelScreen();

	virtual status_t	InitCheck(const char *options) = 0;

	virtual uint16		Width() = 0;
	virtual uint16		Height() = 0;
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual uint8		Depth() = 0;
	virtual lbk_color_space	ColorSpace() = 0;
	virtual status_t	SetHighColor(rgb_color c) = 0;
	virtual status_t	SetLowColor(rgb_color c) = 0;
#endif
	virtual status_t	ConstrainClipping(BRect r, bigtime_t &ts) = 0;
	virtual status_t	FillRect(BRect rect,
					 pattern p,
					 bool patternVertical,
					 bigtime_t &ts) = 0;
	virtual bool		IsFontHeightSupported(uint8 fontHeight) = 0;
	virtual status_t	DrawString(const char *str,
					   BPoint pt,
					   uint8 fontHeight,
					   bool erase,
					   bigtime_t &ts) = 0;
	virtual status_t	MeasureStringWidth(const char *str,
						   uint8 fontHeight,
						   uint16 &width) = 0;
	virtual status_t	InvertRect(BRect rect,
					   bigtime_t &ts) = 0;
	virtual status_t	GetPowerState(bool &state) = 0;
	virtual status_t	SetPowerState(bool state,
					      bigtime_t &ts) = 0;
	virtual status_t	GetTimestamp(bigtime_t &ts) = 0;
	virtual status_t	SetTimestampNow(bigtime_t &tsRet) = 0;
	virtual status_t	DisableUpdate() = 0;
	virtual status_t	EnableUpdate() = 0;
	virtual status_t	MapBuffer(void **buf, size_t *len) = 0;
	virtual status_t	UnmapBuffer() = 0;
	virtual status_t	Flush(bigtime_t &ts) = 0;
	virtual void		Sync() = 0;

	virtual status_t	SetPowerOffTimeout(bigtime_t t) = 0;

	virtual status_t	SendMessage(const BMessage *msg);
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_SCREEN_H__ */

