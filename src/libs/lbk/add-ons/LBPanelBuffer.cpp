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
 * File: LBPanelBuffer.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelBuffer.h>


LBPanelBuffer::LBPanelBuffer()
	: fWidth(0),
	  fHeight(0),
	  fColorSpace(0xff),
	  fBits(NULL),
	  fBitsLength(0)
{
	SetHighColor(255, 255, 255);
	SetLowColor(0, 0, 0);
}


LBPanelBuffer::LBPanelBuffer(uint16 w, uint16 h, lbk_color_space cspace)
	: fWidth(0),
	  fHeight(0),
	  fColorSpace(0xff),
	  fBits(NULL),
	  fBitsLength(0)
{
	SetHighColor(255, 255, 255);
	SetLowColor(0, 0, 0);
	ResizeTo(w, h, cspace);
}


LBPanelBuffer::~LBPanelBuffer()
{
	if(fBits != NULL)
		FreeBitsData(fBits, fBitsLength);
}


void*
LBPanelBuffer::AllocBitsData(size_t len)
{
	return malloc(len);
}


void
LBPanelBuffer::FreeBitsData(void *data, size_t len)
{
	free(data);
}


void
LBPanelBuffer::CopyBitsData(void *dest, const void *src, size_t len)
{
	memcpy(dest, src, len);
}


bool
LBPanelBuffer::ResizeTo(uint16 w, uint16 h, lbk_color_space cspace)
{
	if(w == fWidth && h == fHeight && cspace == fColorSpace)
	{
		if(fBits != NULL) bzero(fBits, fBitsLength);
		return true;
	}

	size_t s = 0;

	switch(cspace)
	{
		case LBK_CS_MONO_Y:
			s = (size_t)(h >> 3);
			if((h & 0x07) != 0) s++;
			s *= (size_t)w;
			break;

		default:
			// TODO
			return false;
	}

	void *bits = NULL;
	if(s > 0)
	{
		bits = AllocBitsData(s);
		if(bits == NULL) return false;
		bzero(bits, s);
	}

	if(fBits != NULL)
		FreeBitsData(fBits, fBitsLength);

	fBits = bits;
	fBitsLength = s;
	fColorSpace = cspace;
	fWidth = w;
	fHeight = h;

	return true;
}


void*
LBPanelBuffer::Bits() const
{
	return fBits;
}


size_t
LBPanelBuffer::BitsLength() const
{
	return fBitsLength;
}


void
LBPanelBuffer::SetBits(const void *data, size_t offset, size_t length)
{
	if(data == NULL || fBits == NULL || offset >= fBitsLength) return;

	if(length > fBitsLength - offset)
		length = fBitsLength - offset;

	CopyBitsData((void*)((uint8*)fBits + offset), data, length);
}


uint16
LBPanelBuffer::Width() const
{
	return fWidth;
}


uint16
LBPanelBuffer::Height() const
{
	return fHeight;
}


BRect
LBPanelBuffer::Bounds() const
{
	return BRect(0, 0, (float)fWidth - 1.f, (float)fHeight - 1.f);
}


uint8
LBPanelBuffer::Depth() const
{
	uint8 depth = 0;

	switch(fColorSpace)
	{
		case LBK_CS_MONO_Y:
		case LBK_CS_MONO_X:
			depth = 1;
			break;

		case LBK_CS_RGB8:
			depth = 8;
			break;

		case LBK_CS_RGB16:
			depth = 16;
			break;

		case LBK_CS_RGB32:
			depth = 32;
			break;

		default:
			break;
	}

	return depth;
}


lbk_color_space
LBPanelBuffer::ColorSpace() const
{
	return fColorSpace;
}


rgb_color
LBPanelBuffer::GetPixel(uint16 x, uint16 y) const
{
	rgb_color c = {0, 0, 0, 0};
	uint32 offset;

	if(x < fWidth && y < fHeight && fBits != NULL)
	{
		switch(fColorSpace)
		{
			case LBK_CS_MONO_Y:
				offset = (uint32)x + (uint32)fWidth * (uint32)(y >> 3);
				if((*(((uint8*)fBits) + offset) & (0x01 << (y & 0x07))) != 0)
					c.red = c.green = c.blue = c.alpha = 255;
				break;

			default:
				// TODO
				break;
		}
	}

	return c;
}


void
LBPanelBuffer::SetPixel(uint16 x, uint16 y, rgb_color c)
{
	uint32 offset;

	if(fBits == NULL || x >= fWidth || y >= fHeight) return;

	switch(fColorSpace)
	{
		case LBK_CS_MONO_Y:
			offset = (uint32)x + (uint32)fWidth * (uint32)(y >> 3);
			if(c.red > 150 && c.green > 150 && c.blue > 150)
				*(((uint8*)fBits) + offset) |= (0x01 << (y & 0x07));
			else
				*(((uint8*)fBits) + offset) &= ~(0x01 << (y & 0x07));
			break;

		default:
			// TODO
			break;
	}
}


rgb_color
LBPanelBuffer::HighColor() const
{
	return fColors[0];
}

rgb_color
LBPanelBuffer::LowColor() const
{
	return fColors[1];
}


void
LBPanelBuffer::SetHighColor(rgb_color c)
{
	*((uint32*)&fColors[0]) = *((uint32*)&c);
}


void
LBPanelBuffer::SetHighColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	fColors[0].red = r;
	fColors[0].green = g;
	fColors[0].blue = b;
	fColors[0].alpha = a;
}


void
LBPanelBuffer::SetLowColor(rgb_color c)
{
	*((uint32*)&fColors[1]) = *((uint32*)&c);
}


void
LBPanelBuffer::SetLowColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	fColors[1].red = r;
	fColors[1].green = g;
	fColors[1].blue = b;
	fColors[1].alpha = a;
}


void
LBPanelBuffer::FillRect(uint16 x, uint16 y, uint16 w, uint16 h, pattern p, bool patternVertical)
{
	if(fBits == NULL || x >= fWidth || y >= fHeight || w == 0 || h == 0) return;

	if(w > fWidth - x)
		w = fWidth - x;
	if(h > fHeight - y)
		h = fHeight - y;

	if(fColorSpace == LBK_CS_MONO_Y)
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

			uint8 *data = (uint8*)fBits + (uint32)x + (uint32)fWidth * (uint32)(cy >> 3);
			for(k = 0; k < w; k++, data++)
			{
				pat = patterns[k & 0x07];
				if(offset > 0)
				{
					pat <<= offset;
					if(m != 8 - offset)
						pat |= (patterns[k & 0x07] >> (8 - offset));
				}

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
LBPanelBuffer::InvertRect(uint16 x, uint16 y, uint16 w, uint16 h)
{
	if(fBits == NULL || x >= fWidth || y >= fHeight || w == 0 || h == 0) return;

	if(w > fWidth - x)
		w = fWidth - x;
	if(h > fHeight - y)
		h = fHeight - y;

	if(fColorSpace == LBK_CS_MONO_Y)
	{
		uint16 k, m;

		uint8 offset = (y & 0x07);
		uint8 mask;
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

			uint8 *data = (uint8*)fBits + (uint32)x + (uint32)fWidth * (uint32)(cy >> 3);
			for(k = 0; k < w; k++, data++)
			{
				uint8 v = *data & mask;
				*data &= ~mask;
				*data |= ~v;
			}
		}
	}
	else
	{
		// TODO
	}
}

