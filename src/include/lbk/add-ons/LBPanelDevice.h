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
 * File: LBPanelDevice.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_H__
#define __LBK_PANEL_DEVICE_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus /* Just for C++ */

// NOTE:
// 	Addon must have C function like below and all the null virtual functions
// 	of class must be implemented.
// 		extern "C" _EXPORT LBPanelDevice* instantiate_panel_device();

class LBApplication;

class _EXPORT LBPanelDevice : public BLocker {
public:
	LBPanelDevice();
	virtual ~LBPanelDevice();

	virtual status_t	InitCheck(const char *options) = 0;

	/* screen */
	virtual uint16		ScreenWidth() = 0;
	virtual uint16		ScreenHeight() = 0;
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual uint8		ScreenDepth() = 0;
	virtual color_space	ScreenColorSpace() = 0;
	virtual status_t	SetHighColor(rgb_color c) = 0;
	virtual status_t	SetLowColor(rgb_color c) = 0;
#endif
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
	virtual status_t	MapBuffer(void **buf) = 0;
	virtual status_t	UnmapBuffer() = 0;
	virtual status_t	Flush(bigtime_t &ts) = 0;
	virtual void		Sync() = 0;

	/* keys */
	virtual status_t	GetCountOfKeys(uint8 &count) = 0;
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual status_t	GetOrientationOfKeys(orientation &o) = 0;
	virtual status_t	GetSideOfKeys(bool &right_or_bottom) = 0;
	virtual status_t	GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
						      uint16 &OffsetRightBottom) = 0;
#endif

	void			SendMessageToApp(const BMessage *msg);
	void			SendMessageToApp(uint32 command);

private:
	friend class LBApplication;

	int32 fID;
	BMessenger fMsgr;
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_H__ */

