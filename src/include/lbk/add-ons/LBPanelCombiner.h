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
 * File: LBPanelCombiner.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_COMBINER_H__
#define __LBK_PANEL_DEVICE_COMBINER_H__

#include <lbk/add-ons/LBPanelScreen.h>
#include <lbk/add-ons/LBPanelKeypad.h>
#include <lbk/add-ons/LBPanelTouchpad.h>

#ifdef __cplusplus /* Just for C++ */

enum {
	LBK_SCREEN_COMBINE_BY_SINGLE = 0x00000000,
	LBK_SCREEN_COMBINE_BY_JOIN_TOGETHER,
};

class _EXPORT LBPanelCombiner : public LBPanelDevice {
public:
	LBPanelCombiner();
	virtual ~LBPanelCombiner();

	status_t		SetCombineStyle(uint32 style);

	status_t		AddScreen(const char *add_on, BPoint location);
	status_t		AddScreen(LBPanelScreen *screen, BPoint location);

	status_t		AddKeypad(const char *add_on);
	status_t		AddKeypad(LBPanelKeypad *keypad);

	status_t		AddTouchpad(const char *add_on);
	status_t		AddTouchpad(LBPanelTouchpad *touchpad);

	virtual status_t	InitCheck(const char *options);

	/* screen */
	virtual uint16		ScreenWidth();
	virtual uint16		ScreenHeight();
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual uint8		ScreenDepth();
	virtual lbk_color_space	ScreenColorSpace();
	virtual status_t	SetHighColor(rgb_color c);
	virtual status_t	SetLowColor(rgb_color c);
#endif
	virtual status_t	ConstrainClipping(BRect r, bigtime_t &ts);
	virtual status_t	FillRect(BRect rect,
					 pattern p,
					 bool patternVertical,
					 bigtime_t &ts);
	virtual bool		IsFontHeightSupported(uint8 fontHeight);
	virtual status_t	DrawString(const char *str,
					   BPoint pt,
					   uint8 fontHeight,
					   bool erase,
					   bigtime_t &ts);
	virtual status_t	MeasureStringWidth(const char *str,
						   uint8 fontHeight,
						   uint16 &width);
	virtual status_t	InvertRect(BRect rect,
					   bigtime_t &ts);
	virtual status_t	GetPowerState(bool &state);
	virtual status_t	SetPowerState(bool state,
					      bigtime_t &ts);
	virtual status_t	GetTimestamp(bigtime_t &ts);
	virtual status_t	SetTimestampNow(bigtime_t &tsRet);
	virtual status_t	DisableUpdate();
	virtual status_t	EnableUpdate();
	virtual status_t	MapBuffer(void **buf, size_t *len);
	virtual status_t	UnmapBuffer();
	virtual status_t	Flush(bigtime_t &ts);
	virtual void		Sync();

	virtual status_t	SetPowerOffTimeout(bigtime_t t);

	/* keys */
	virtual status_t	GetCountOfKeys(uint8 &count);
	virtual status_t	BlockKeyEvents(bool state);
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual status_t	GetOrientationOfKeys(orientation &o);
	virtual status_t	GetSideOfKeys(bool &right_or_bottom);
	virtual status_t	GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
						      uint16 &OffsetRightBottom);
#endif

	virtual status_t	SendMessage(const BMessage *msg);

private:
	BList fScreens;
	BList fKeypads;
	BList fTouchpads;

	uint32 fCombineStyle;

	uint16 fWidth;
	uint16 fHeight;
	uint8 fKeysCount;
	bool fState;
	bool fBlockKeyEvents;
	bigtime_t fBlockTimestamp;
	bigtime_t fTimestamp;
#ifdef LBK_ENABLE_MORE_FEATURES
	uint8 fDepth;
	lbk_color_space fColorSpace;
	orientation fOrientation;
	bool fKeysRB;
	uint16 fKeysOffset[2];
#endif

	virtual void		Init(int32 id, const BMessenger &msgr);
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_COMBINER_H__ */

