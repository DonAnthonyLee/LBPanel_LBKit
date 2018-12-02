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

#include <lbk/LBAppDefs.h>
#include <lbk/add-ons/LBPanelBuffer.h>

#ifdef ETK_MAJOR_VERSION
	#ifdef ETK_BIG_ENDIAN
		#define B_HOST_IS_BENDIAN	1
	#else
		#define B_HOST_IS_BENDIAN	0
	#endif
	#define B_SWAP_INT16(v)			E_SWAP_INT16(v)
	#define B_SWAP_INT32(v)			E_SWAP_INT32(v)
#endif


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

		case LBK_CS_RGB565_BIG:
			s = 2 * w * h;
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

		case LBK_CS_RGB332:
			depth = 8;
			break;

		case LBK_CS_RGB565_BIG:
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
	rgb_color c = LowColor();
	uint32 offset;
	uint32 v;

	if(x < fWidth && y < fHeight && fBits != NULL)
	{
		switch(fColorSpace)
		{
			case LBK_CS_MONO_Y:
				offset = (uint32)x + (uint32)fWidth * (uint32)(y >> 3);
				if((*((uint8*)fBits + offset) & (0x01 << (y & 0x07))) != 0)
					c = HighColor();
				break;

			case LBK_CS_RGB565_BIG:
				offset = (uint32)x + (uint32)fWidth * (uint32)y;
#if B_HOST_IS_BENDIAN
				v = (uint32)(*((uint16*)fBits + offset));
#else
				v = (uint32)B_SWAP_INT16(*((uint16*)fBits + offset));
#endif
				c.set_to(((v & 0xf800) >> 8) | 0x0007,
					 ((v & 0x07e0) >> 3) | 0x0003,
					 ((v & 0x001f) << 3) | 0x0007);
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
				*((uint8*)fBits + offset) |= (0x01 << (y & 0x07));
			else
				*((uint8*)fBits + offset) &= ~(0x01 << (y & 0x07));
			break;

		case LBK_CS_RGB565_BIG:
			offset = (uint32)x + (uint32)fWidth * (uint32)y;
#if B_HOST_IS_BENDIAN
			*((uint16*)fBits + offset) = ((uint16)(c.red >> 3) << 11) |
						     ((uint16)(c.green >> 2) << 5) |
						     (uint16)(c.blue >> 3);
#else
			*((uint16*)fBits + offset) = ((uint16)(c.red >> 3) << 3) |
						     ((uint16)((c.green >> 2) & 0x03) << 13) |
						     (uint16)(c.green >> 5) |
						     (uint16)((c.blue >> 3) << 8);
#endif
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
	uint16 k, m;
	uint8 patterns[8];

	if(fBits == NULL || x >= fWidth || y >= fHeight || w == 0 || h == 0) return;

	if(w > fWidth - x)
		w = fWidth - x;
	if(h > fHeight - y)
		h = fHeight - y;

	if(patternVertical)
	{
		memcpy(patterns, p.data, sizeof(patterns));
	}
	else for(k = 0; k < 8; k++)
	{
		if(p.data[k] == 0x00 || p.data[k] == 0xff)
			patterns[k] = p.data[k];
		else for(m = 0, patterns[k] = 0; m < 8; m++)
			patterns[k] |= ((p.data[m] >> (7 - k + m)) & (0x01 << m));
	}

	if(fColorSpace == LBK_CS_MONO_Y)
	{
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
	else if(fColorSpace == LBK_CS_RGB565_BIG)
	{
		for(m = 0; m < h; m++)
		{
			uint16 *data = (uint16*)fBits + (uint32)x + (uint32)fWidth * (uint32)(y + m);
			for(k = 0; k < w; k++)
			{
				rgb_color c = ((patterns[k & 0x07] & (0x01 << (m & 0x07))) == 0) ? LowColor() : HighColor();
#if B_HOST_IS_BENDIAN
				*data++ = ((uint16)(c.red >> 3) << 11) |
					  ((uint16)(c.green >> 2) << 5) |
					  (uint16)(c.blue >> 3);
#else
				*data++ = ((uint16)(c.red >> 3) << 3) |
					  ((uint16)((c.green >> 2) & 0x03) << 13) |
					  (uint16)(c.green >> 5) |
					  (uint16)((c.blue >> 3) << 8);
#endif
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
				*data |= (~v & mask);
			}
		}
	}
	else if(fColorSpace == LBK_CS_RGB565_BIG)
	{
		uint16 k, m;

		for(m = 0; m < h; m++)
		{
			uint16 *data = (uint16*)fBits + (uint32)x + (uint32)fWidth * (uint32)(y + m);
			for(k = 0; k < w; k++, data++) *data = (uint16)~(*data);
		}
	}
	else
	{
		// TODO
	}
}

