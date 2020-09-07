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
 * File: LCDScreen.cpp
 * Description: LCD ST7735S Screen Add-on for LBKit
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <time.h>

#include <stdint.h>
#include <lcd_st7735s_ioctl.h>

#include "LCDScreen.h"


extern "C" _EXPORT LBPanelScreen* instantiate_panel_screen()
{
	return new LCDScreen();
}


LCDScreen::LCDScreen()
	: LBPanelScreen(),
	  fFD(-1),
	  fBuffer(NULL),
	  fSwapXY(false), fWidth(0), fHeight(0)
{
	fColors[0].red = 0xff;
	fColors[0].green = 0xcb;
	fColors[0].blue = 0;
	fColors[1].red = fColors[1].green = fColors[1].blue = 0;
}


LCDScreen::~LCDScreen()
{
	if(fBuffer != NULL)
		munmap(fBuffer, 2 * (size_t)fWidth * (size_t)fHeight);

	if(fFD >= 0)
		close(fFD);
}


status_t
LCDScreen::InitCheck(const char *options)
{
	if(fFD == -1)
	{
		BString opt(options);
		const char *dev = DEFAULT_LCD_DEVICE;

		if(opt.FindFirst("dev=") == 0 && opt.Length() > 4)
		{
			BEntry entry(opt.String() + 4);
			if(entry.Exists() == false)
			{
				fprintf(stderr, "[LCDScreen]: No such device (%s) !\n", opt.String() + 4);
				return B_ERROR;
			}

			dev = opt.String() + 4;
		}

		_lcd_st7735s_prop_t prop;
		bzero(&prop, sizeof(prop));

		if((fFD = open(dev, O_RDWR)) < 0 ||
		    ioctl(fFD, LCD_ST7735S_IOC_GET_PROP, &prop) != 0)
		{
			if(fFD >= 0) close(fFD);
			perror("[LCDScreen]: Unable to open device !\n");
			fFD = -2;
		}
		else
		{
			fSwapXY = (prop.swap_xy == 1);
			fWidth = prop.swap_xy ? prop.h : prop.w;
			fHeight = prop.swap_xy ? prop.w : prop.h;
		}
	}

	return((fFD < 0) ? B_ERROR : B_OK);
}


uint16
LCDScreen::Width()
{
	return fWidth;
}


uint16
LCDScreen::Height()
{
	return fHeight;
}


status_t
LCDScreen::ConstrainClipping(BRect r, bigtime_t &ts)
{
	_lcd_st7735s_set_clipping_t data;

	bzero(&data, sizeof(data));

	if(r.IsValid() && r.left >= 0 && r.top >= 0 &&
	   r.Width() < fWidth && r.Height() < fHeight)
	{
		data.x = (uint8_t)r.left;
		data.y = (uint8_t)r.top;
		data.w = (uint8_t)(r.Width() + 1);
		data.h = (uint8_t)(r.Height() + 1);
	}

	if(ioctl(fFD, LCD_ST7735S_IOC_SET_CLIPPING, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
LCDScreen::FillRect(BRect r,
		    pattern p,
		    bool patternVertical,
		    bigtime_t &ts)
{
	_lcd_st7735s_clear_t data;

	if(r.IsValid() == false) return B_BAD_VALUE;
	if(r.left < 0 || r.top < 0 ||
	   r.Width() >= fWidth ||
	   r.Height() >= fHeight) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (uint8_t)r.left;
	data.y = (uint8_t)r.top;
	data.w = (uint8_t)(r.Width() + 1);
	data.h = (uint8_t)(r.Height() + 1);

	if(patternVertical)
	{
		memcpy(data.patterns, p.data, sizeof(data.patterns));
	}
	else for(int k = 0; k < 8; k++)
	{
		if(p.data[k] == 0x00 || p.data[k] == 0xff)
			data.patterns[k] = p.data[k];
		else for(int m = 0; m < 8; m++)
			data.patterns[k] |= ((p.data[m] >> (7 - k + m)) & (0x01 << m));
	}

	if(ioctl(fFD, LCD_ST7735S_IOC_CLEAR, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


bool
LCDScreen::IsFontHeightSupported(uint8 size)
{
	if(size == 8 || size == 12 || size == 14 || size == 16 || size == 24 || size == 32) return true;
	return false;
}


status_t
LCDScreen::DrawString(const char *str,
		      BPoint pt,
		      uint8 fontHeight,
		      bool use_low_color,
		      bigtime_t &ts)
{
	_lcd_st7735s_draw_str_t data;
	rgb_color c = fColors[use_low_color ? 1 : 0];
	if(str == NULL || *str == 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (int16_t)pt.x;
	data.y = (int16_t)pt.y;
	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));
	data.color = ((uint16)(c.red >> 3)) << 11;
	data.color |= ((uint16)(c.green >> 2)) << 5;
	data.color |= (uint16)(c.blue >> 3);

	if(ioctl(fFD, LCD_ST7735S_IOC_DRAW_STRING, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
LCDScreen::MeasureStringWidth(const char *str,
			      uint8 fontHeight,
			      uint16 &width)
{
	_lcd_st7735s_string_width_t data;

	if(str == NULL || *str == 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));

	if(ioctl(fFD, LCD_ST7735S_IOC_STRING_WIDTH, &data) != 0) return B_ERROR;
	width = data.w;
	return B_OK;
}


status_t
LCDScreen::InvertRect(BRect r,
		      bigtime_t &ts)
{
	_lcd_st7735s_invert_t data;

	if(r.IsValid() == false) return B_BAD_VALUE;
	if(r.left < 0 || r.top < 0 || r.Width() >= fWidth || r.Height() >= fHeight) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (uint8_t)r.left;
	data.y = (uint8_t)r.top;
	data.w = (uint8_t)(r.Width() + 1);
	data.h = (uint8_t)(r.Height() + 1);

	if(ioctl(fFD, LCD_ST7735S_IOC_INVERT, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
LCDScreen::GetPowerState(bool &state)
{
	_lcd_st7735s_power_t data;
	bzero(&data, sizeof(data));

	data.state = 2;

	if(ioctl(fFD, LCD_ST7735S_IOC_POWER, &data) != 0) return B_ERROR;
	state = (data.state == 1);
	return B_OK;
}


status_t
LCDScreen::SetPowerState(bool state, bigtime_t &ts)
{
	_lcd_st7735s_power_t data;
	bzero(&data, sizeof(data));

	data.state = (state ? 1 : 0);
	if(ioctl(fFD, LCD_ST7735S_IOC_POWER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
LCDScreen::GetTimestamp(bigtime_t &ts)
{
	_lcd_st7735s_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 0;

	if(ioctl(fFD, LCD_ST7735S_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
LCDScreen::SetTimestampNow(bigtime_t &tsRet)
{
	_lcd_st7735s_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 1;

	if(ioctl(fFD, LCD_ST7735S_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	tsRet = data.ts;
	return B_OK;
}

status_t
LCDScreen::DisableUpdate()
{
	uint8_t st = 0;

	if(ioctl(fFD, LCD_ST7735S_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


status_t
LCDScreen::EnableUpdate()
{
	uint8_t st = 1;

	if(ioctl(fFD, LCD_ST7735S_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
uint8
LCDScreen::Depth()
{
	return 16;
}


lbk_color_space
LCDScreen::ColorSpace()
{
	return LBK_CS_RGB565_BIG;
}


status_t
LCDScreen::SetHighColor(rgb_color c)
{
	uint16 color;

	*((uint32*)&fColors[0]) = *((uint32*)&c);

	color = ((uint16)(c.red >> 3)) << 11;
	color |= ((uint16)(c.green >> 2)) << 5;
	color |= (uint16)(c.blue >> 3);

	if(ioctl(fFD, LCD_ST7735S_IOC_SET_HIGH_COLOR, &color) != 0) return B_ERROR;
	return B_OK;
}


status_t
LCDScreen::SetLowColor(rgb_color c)
{
	uint16 color;

	*((uint32*)&fColors[1]) = *((uint32*)&c);

	color = ((uint16)(c.red >> 3)) << 11;
	color |= ((uint16)(c.green >> 2)) << 5;
	color |= (uint16)(c.blue >> 3);

	if(ioctl(fFD, LCD_ST7735S_IOC_SET_LOW_COLOR, &color) != 0) return B_ERROR;
	return B_OK;
}
#endif


status_t
LCDScreen::MapBuffer(void **buf, size_t *len)
{
	EAutolock <LBPanelDevice>autolock(Panel());
	if(autolock.IsLocked() == false) return B_ERROR;

	if(buf == NULL) return B_BAD_VALUE;
	if(fBuffer != NULL) return B_ERROR;

	fBuffer = mmap(NULL,
		       2 * (size_t)fWidth * (size_t)fHeight,
		       PROT_READ | PROT_WRITE,
		       MAP_SHARED,
		       fFD,
		       0);
	if(fBuffer == NULL || fBuffer == MAP_FAILED)
	{
		fBuffer = NULL;
		return B_ERROR;
	}

	*buf = fBuffer;
	*len = 2 * (size_t)fWidth * (size_t)fHeight;
	return B_OK;
}


status_t
LCDScreen::UnmapBuffer()
{
	EAutolock <LBPanelDevice>autolock(Panel());
	if(autolock.IsLocked() == false) return B_ERROR;

	if(fBuffer == NULL) return B_BAD_VALUE;
	if(munmap(fBuffer, 2 * (size_t)fWidth * (size_t)fHeight) != 0) return B_ERROR;
	fBuffer = NULL;
	return B_OK;
}


status_t
LCDScreen::Flush(bigtime_t &ts)
{
	_lcd_st7735s_buffer_t data;
	bzero(&data, sizeof(data));

	data.action = 1;

	if(ioctl(fFD, LCD_ST7735S_IOC_BUFFER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


void
LCDScreen::Sync()
{
	// Because it's not client/server mode, we don't need to sync
}


status_t
LCDScreen::SetPowerOffTimeout(bigtime_t t)
{
	uint64_t data = (t > 0 ? (uint64_t)t : 0);

	if(ioctl(fFD, LCD_ST7735S_IOC_SET_OFF_TIMEOUT, &data) != 0) return B_ERROR;
	return B_OK;
}

