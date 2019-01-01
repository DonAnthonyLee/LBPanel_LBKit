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
 * File: LBPanelScreenNet.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_SCREEN_NET_H__
#define __LBK_PANEL_DEVICE_SCREEN_NET_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus /* Just for C++ */

#ifdef ETK_MAJOR_VERSION
	#define UDPMessenger	ENetMessenger
#elif (B_BEOS_VERSION >= B_BEOS_VERSION_DANO)
	class UDPMessenger;
#else
	#error "class UDPMessenger wasn't implemented !"
#endif

class _EXPORT LBPanelScreenUDP : public LBPanelScreen {
public:
	LBPanelScreenUDP();
	virtual ~LBPanelScreenUDP();

	void			SetTarget(BNetAddress *target);

	virtual status_t	InitCheck(const char *options);

	virtual uint16		Width();
	virtual uint16		Height();
#ifdef LBK_ENABLE_MORE_FEATURES
	virtual uint8		Depth();
	virtual color_space	ColorSpace();
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

private:
	UDPMessenger *fNetMsgr;
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_SCREEN_NET_H__ */

