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
 * File: LBPanelBuffer.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_BUFFER__
#define __LBK_PANEL_BUFFER__

#include <be/Be.h>

#ifdef __cplusplus /* Just for C++ */

enum {
	LBK_CS_MONO_Y = 0,
	LBK_CS_MONO_X,

	LBK_CS_RGB332,		// RGB(3:3:2)
	LBK_CS_RGB565_BIG,	// RGB(5:6:5)
	LBK_CS_RGB32,		// BGRx(8:8:8:8)
};

typedef uint8	lbk_color_space;

class LBPanelBuffer {
public:
	LBPanelBuffer();
	LBPanelBuffer(uint16 w, uint16 h, lbk_color_space cspace);
	virtual ~LBPanelBuffer();

	bool			ResizeTo(uint16 w, uint16 h, lbk_color_space cspace);

	void*			Bits() const;
	size_t			BitsLength() const;

	void			SetBits(const void *data, size_t offset, size_t length);

	uint16			Width() const;
	uint16			Height() const;
	BRect			Bounds() const;

	uint8			Depth() const;
	lbk_color_space		ColorSpace() const;

	void			SetPixel(uint16 x, uint16 y, rgb_color c);
	rgb_color		GetPixel(uint16 x, uint16 y) const;

	// HighColor/LowColor: for pattern
	rgb_color		HighColor() const;
	rgb_color		LowColor() const;
	void			SetHighColor(rgb_color c);
	void			SetHighColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	void			SetLowColor(rgb_color c);
	void			SetLowColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);

	void			FillRect(uint16 x, uint16 y,
					 uint16 w, uint16 h,
					 pattern p, bool patternVertical = false);
	void			InvertRect(uint16 x, uint16 y,
					   uint16 w, uint16 h);

private:
	uint16 fWidth;
	uint16 fHeight;
	lbk_color_space fColorSpace;
	void *fBits;
	size_t fBitsLength;
	rgb_color fColors[2];

	virtual void*		AllocBitsData(size_t len);
	virtual void		FreeBitsData(void *data, size_t len);
	virtual void		CopyBitsData(void *dest, const void *src, size_t len);
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_BUFFER__ */

