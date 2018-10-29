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
 * File: VPDView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_VIRTUAL_PANEL_DEVICE_VIEW_H__
#define __LBK_VIRTUAL_PANEL_DEVICE_VIEW_H__

#include <be/Be.h>

#include <lbk/add-ons/LBPanelBuffer.h>

#ifdef __cplusplus /* Just for C++ */

class VPDView : public BView {
public:
	VPDView(BRect frame, const char *name, uint32 resizingMode);
	virtual ~VPDView();

	void			SetPointSize(uint8 s);
	void			SetLabel(const char *str);

	bool			SetFontHeight(uint8 h);
	void			SetPowerState(bool state);

	uint8*			Buffer() const;
	size_t			BufferLength() const;
	void			ResizeBuffer(uint16 w, uint16 h, lbk_color_space cspace);

	BRect			FillRectOnBuffer(uint16 x, uint16 y,
						 uint16 w, uint16 h,
						 pattern p, bool patternVertical = false);
	BRect			DrawStringOnBuffer(const char *str,
						   uint16 x, uint16 y,
						   bool erase_mode = false);

	virtual void		GetPreferredSize(float *width, float *height);
	virtual void		Draw(BRect updateRect);
	virtual void		MessageReceived(BMessage *msg);

private:
	LBPanelBuffer fBuffer;
	uint8 fPointSize;
	char *fLabel;
	bool fPowerState;
	bool fBufferUpdateEnabled;
	BRect fBufferUpdateRect;

	BRect			DrawingArea(uint16 x, uint16 y, uint16 w, uint16 h) const;
	uint16			StringWidth_h8(uint16 c) const;
	uint16			StringWidth_h8(const char *str) const;
	BRect			DrawStringOnBuffer_h8(const char *str, uint16 x, uint16 y, bool erase_mode);
};

#endif /* __cplusplus */

#endif /* __LBK_VIRTUAL_PANEL_DEVICE_VIEW_H__ */

