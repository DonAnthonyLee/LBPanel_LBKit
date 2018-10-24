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

#ifdef ETK_MAJOR_VERSION
	#ifdef ETK_BIG_ENDIAN
		#define B_HOST_IS_BENDIAN
	#endif
#endif


VPDView::VPDView(BRect frame, const char* title, uint32 resizingMode)
	: BView(frame, title, resizingMode, B_WILL_DRAW),
	  fWidth(0),
	  fHeight(0),
	  fPointSize(1),
	  fDepth(1),
	  fLabel(NULL),
	  fBuffer(NULL),
	  fBufferLength(0),
	  fPowerState(true)
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


bool
VPDView::SetFontHeight(uint8 h)
{
	if(h < 8 || h > 48) return false;

	BFont font;
	font_height fontHeight;

	GetFont(&font);

	float t = h - ((h < 24) ? 4 : 8);
	for(int k = 0; k < 8; k++)
	{
		font.SetSize(t + k);
		font.GetHeight(&fontHeight);

		if(fontHeight.ascent + fontHeight.descent >= (float)h ||
		   (float)h - (fontHeight.ascent + fontHeight.descent) < 1.1f)
		{
			SetFont(&font, B_FONT_SIZE);
			return true;
		}
	}

	return false;
}


void
VPDView::SetPowerState(bool state)
{
	if(fPowerState != state)
	{
		fPowerState = state;
		Invalidate();
	}
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
	float space = (fPointSize < 2) ? 0 : ((fPointSize < 4) ? 1 : 2);

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
					c.red = c.green = c.blue = (fPowerState ? 50 : 200);
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
	uint16 space = (fPointSize < 2) ? 0 : ((fPointSize < 4) ? 1 : 2);

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
				SetHighColor(c);
				FillRect(r, B_SOLID_HIGH);
			}
			r.OffsetBy(r.Width() + 1 + space, 0);
		}

		r.left = 0;
		r.right = fPointSize - 1;
		r.OffsetBy(0, r.Height() + 1 + space);
	}

	if(fLabel != NULL || fPowerState == false)
	{
		BString str(fPowerState ? fLabel : "Power Off");
		BFont font;

		PushState();

		GetFont(&font);
		font.SetSize(Bounds().Height() / (fPowerState ? 2 : 3));
		SetFont(&font, B_FONT_SIZE);

		font_height fontHeight;
		font.GetHeight(&fontHeight);

		float w = font.StringWidth(str.String());
		float h = fontHeight.ascent + fontHeight.descent;

		r = Bounds();
		r.InsetBy((Bounds().Width() - w) / 2.f, (Bounds().Height() - h) / 2.f);
		if(r.Intersects(updateRect))
		{
			SetDrawingMode(B_OP_ALPHA);
			SetHighColor(100, 100, 100, 150);
			MovePenTo(r.LeftTop() + BPoint(0, fontHeight.ascent));
			DrawString(str.String());
		}

		PopState();
	}
}


void
VPDView::FillRectOnBuffer(uint16 x, uint16 y, uint16 w, uint16 h, pattern p, bool patternVertical)
{
	if(fBuffer == NULL || x >= fWidth || y >= fHeight || w == 0 || h == 0) return;

	if(w > fWidth - x) w = fWidth - x;
	if(h > fHeight - y) h = fHeight - y;

	if(fDepth == 1)
	{
		uint16 k, m;

		uint8 patterns[8];
		if(patternVertical)
		{
			memcpy(patterns, p.data, sizeof(patterns));
		}
		else for(k = 0; k < 8; k++)
		{
			if(p.data[k] == 0x00 || p.data[k] == 0xff)
				patterns[k] = p.data[k];
			else for(m = 0; m < 8; m++)
				patterns[k] |= ((p.data[m] >> (7 - k + m)) & (0x01 << m));
		}

		uint8 offset = (y & 0x07);
		uint8 pat, mask;
		uint16 cy;

		for(m = 0; m < h;)
		{
			cy = y + m;

			mask = 0xff;
			if(m == 0 && offset > 0)
			{
				mask <<= offset;
				m += (8 - offset);
			}
			else
			{
				m += 8;
			}

			if(m > h)
				mask &= ~(0xff << (8 - (m - h)));

			for(k = 0; k < w; k++)
			{
				pat = patterns[k & 0x07];
				if(offset > 0)
				{
					if(m == 8 - offset)
					{
						pat <<= offset;
					}
					else
					{
						pat <<= offset;
						pat |= (patterns[k & 0x07] >> (8 - offset));
					}
				}

				uint8 *data = fBuffer + (uint32)(x + k) + (uint32)fWidth * (uint32)(cy >> 3);
				*data &= ~mask;
				*data |= (pat & mask);
			}
		}
	}
	else
	{
		// TODO
	}
}


void
VPDView::DrawStringOnBuffer(const char *str, uint16 x, uint16 y, bool erase_mode)
{
	if(fBuffer == NULL || str == NULL || *str == 0 || x >= fWidth || y >= fHeight) return;

	BFont font;
	font_height fontHeight;

	GetFont(&font);
	font.GetHeight(&fontHeight);

	uint16 w = (uint16)font.StringWidth(str);
	uint16 h = (uint16)(fontHeight.ascent + fontHeight.descent);
	if(w > fWidth - x) w = fWidth - x;
	if(h > fHeight - y) h = fHeight - y;

	BBitmap *bitmap = new BBitmap(BRect(0, 0, w - 1, h - 1), B_RGB32);
	BView *view = new BView(bitmap->Bounds(), NULL, B_FOLLOW_ALL, 0);

	view->SetFont(&font, B_FONT_ALL);
	if(fDepth == 1)
		view->ForceFontAliasing(false);
	view->SetHighColor(0, 0, 0);
	view->SetLowColor(255, 255, 255);

	bitmap->Lock();
	bitmap->AddChild(view);
	view->FillRect(view->Bounds(), B_SOLID_LOW);
	view->MovePenTo(0, fontHeight.ascent);
	view->DrawString(str);
	view->Flush();
	view->Sync();
	bitmap->Unlock();

	uint32 *data;
#ifdef ETK_MAJOR_VERSION
	EPixmap *pixmap = new EPixmap(bitmap->Bounds(), B_RGB32);
	bitmap->Lock();
	bitmap->GetPixmap(pixmap, bitmap->Bounds());
	bitmap->Unlock();
	data = (uint32*)pixmap->Bits();
#else
	data = (uint32*)bitmap->Bits();
#endif

	if(data != NULL)
	{
		for(uint16 yy = 0; yy < h; yy++)
		{
			const uint32 *bits = (const uint32*)(data + yy * w);

			for(uint16 xx = 0; xx < w; xx++)
			{
				rgb_color c;
				uint32 v = *bits++;
#ifdef B_HOST_IS_BENDIAN
				c.red = (v >> 8) & 0xff;
				c.green = (v >> 16) & 0xff;
				c.blue = (v >> 24) & 0xff;
#else
				c.red = (v >> 16) & 0xff;
				c.green = (v >> 8) & 0xff;
				c.blue = v & 0xff;
#endif

				switch(fDepth)
				{
					case 1:
						if(c.red > 150 && c.green > 150 && c.blue > 150) break;
						FillRectOnBuffer(x + xx, y + yy, 1, 1, erase_mode ? B_SOLID_LOW : B_SOLID_HIGH, true);
						break;

					default:
						// TODO
						break;
				}
			}
		}
	}

#ifdef ETK_MAJOR_VERSION
	delete pixmap;
#endif
	delete bitmap;
}

