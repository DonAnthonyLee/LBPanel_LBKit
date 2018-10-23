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
 * File: VPDView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "VPDView.h"


VPDView::VPDView(BRect frame, const char* title, uint32 resizingMode)
	: BView(frame, title, resizingMode, B_WILL_DRAW),
	  fWidth(0),
	  fHeight(0),
	  fPointSize(1),
	  fDepth(1),
	  fLabel(NULL),
	  fBuffer(NULL)
{
	SetViewColor(230, 230, 230);
	SetLowColor(230, 230, 230);
}


VPDView::~VPDView()
{
	if(fLabel != NULL) free(fLabel);
	if(fBuffer != NULL) free(fBuffer);
}


void
VPDView::SetWidth(uint16 w)
{
	if(fWidth != w)
	{
		fWidth = w;
		ResizeBuffer();
	}
}


void
VPDView::SetHeight(uint16 h)
{
	if(fHeight != h)
	{
		fHeight = h;
		ResizeBuffer();
	}
}


void
VPDView::SetPointSize(uint8 s)
{
	if(fPointSize != s && s > 0)
	{
		fPointSize = s;
		Invalidate();
	}
}


void
VPDView::SetLabel(const char *str)
{
	if(fLabel != NULL)
	{
		free(fLabel);
		fLabel = NULL;
	}

	if(!(str == NULL || *str == 0))
		fLabel = strdup(str);

	Invalidate();
}


void
VPDView::ResizeBuffer()
{
	if(fBuffer != NULL)
	{
		free(fBuffer);
		fBuffer = NULL;
		fBufferLength = 0;
	}

	if(fWidth > 0 && fHeight > 0)
	{
		size_t s = 0;

		switch(fDepth)
		{
			case 1:
				s = (size_t)(fHeight >> 3);
				if((fHeight & 0x07) != 0) s++;
				s *= (size_t)fWidth;
				break;

			default:
				// TODO
				break;
		}

		if(s > 0)
		{
			fBuffer = (uint8*)malloc(s);
			if(fBuffer != NULL)
			{
				bzero(fBuffer, s);
				fBufferLength = s;
			}
		}
	}
}


uint8*
VPDView::Buffer() const
{
	return fBuffer;
}


size_t
VPDView::BufferLength() const
{
	return fBufferLength;
}


void
VPDView::GetPreferredSize(float *width, float *height)
{
	float w = 0, h = 0;
	float space = (fPointSize < 4) ? 0 : 2;

	if(fWidth > 0)
		w = (float)fWidth * (float)fPointSize + (float)(fWidth - 1) * (float)space;

	if(fHeight > 0)
		h = (float)fHeight * (float)fPointSize + (float)(fHeight - 1) * (float)space;

	if(width != NULL) *width = w;
	if(height != NULL) *height = h;
}


rgb_color
VPDView::PixelAt(uint16 x, uint16 y)
{
	rgb_color c = ViewColor();
	uint32 offset;

	if(x < fWidth && y < fHeight && fBuffer != NULL)
	{
		switch(fDepth)
		{
			case 1:
				offset = (uint32)x + (uint32)fWidth * (uint32)(y >> 3);
				if((*(fBuffer + offset) & (0x01 << (y & 0x07))) != 0)
					c.red = c.green = c.blue = 50;
				break;

			default:
				// TODO
				break;
		}
	}

	return c;
}


void
VPDView::Draw(BRect updateRect)
{
	BRect r;
	uint16 space = (fPointSize < 4) ? 0 : 2;

#if 1
	if(fDepth != 1) return;
#endif

	r.Set(0, 0, fPointSize - 1, fPointSize - 1);
	for(uint16 y = 0; y < fHeight; y++)
	{
		for(uint16 x = 0; x < fWidth; x++)
		{
			if(r.Intersects(updateRect))
			{
				rgb_color c = PixelAt(x, y);
				if(fLabel != NULL) c.alpha = 200;
				SetHighColor(c);
				FillRect(r, B_SOLID_HIGH);
			}
			r.OffsetBy(r.Width() + 1 + space, 0);
		}

		r.left = 0;
		r.right = fPointSize - 1;
		r.OffsetBy(0, r.Height() + 1 + space);
	}

	if(fLabel != NULL)
	{
		BFont font;

		PushState();

		GetFont(&font);
		font.SetSize(Bounds().Height() / 2);
		SetFont(&font, B_FONT_SIZE);

		font_height fontHeight;
		font.GetHeight(&fontHeight);

		float w = font.StringWidth(fLabel);
		float h = fontHeight.ascent + fontHeight.descent;

		r = Bounds();
		r.InsetBy((Bounds().Width() - w) / 2.f, (Bounds().Height() - h) / 2.f);
		if(r.Intersects(updateRect))
		{
			SetDrawingMode(B_OP_ALPHA);
			SetHighColor(100, 100, 100, 150);
			MovePenTo(r.LeftTop() + BPoint(0, fontHeight.ascent));
			DrawString(fLabel);
		}

		PopState();
	}
}


