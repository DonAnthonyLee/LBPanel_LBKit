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
 * File: OLEDScreen.cpp
 * Description: OLED SSD1306 Screen Add-on for LBKit
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
#include <oled_ssd1306_ioctl.h>

#include "OLEDScreen.h"


extern "C" _EXPORT LBPanelScreen* instantiate_panel_screen()
{
	return new OLEDScreen();
}


OLEDScreen::OLEDScreen()
	: LBPanelScreen(),
	  fFD(-1),
	  fBuffer(NULL)
{
}


OLEDScreen::~OLEDScreen()
{
	if(fBuffer != NULL)
		munmap(fBuffer, OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3));

	if(fFD >= 0)
		close(fFD);
}


status_t
OLEDScreen::InitCheck(const char *options)
{
	if(fFD == -1)
	{
		BString opt(options);
		const char *dev = DEFAULT_OLED_DEVICE;

		if(opt.FindFirst("dev=") == 0 && opt.Length() > 4)
		{
			BEntry entry(opt.String() + 4);
			if(entry.Exists() == false)
			{
				fprintf(stderr, "[OLEDScreen]: No such device (%s) !", opt.String() + 4);
				return B_ERROR;
			}

			dev = opt.String() + 4;
		}

		if((fFD = open(dev, O_RDWR)) < 0)
		{
			perror("[OLEDScreen]: Unable to open device !");
			fFD = -2;
		}
	}

	return((fFD < 0) ? B_ERROR : B_OK);
}


uint16
OLEDScreen::Width()
{
	return OLED_SCREEN_WIDTH;
}


uint16
OLEDScreen::Height()
{
	return OLED_SCREEN_HEIGHT;
}


status_t
OLEDScreen::ConstrainClipping(BRect r, bigtime_t &ts)
{
	_oled_ssd1306_set_clipping_t data;

	bzero(&data, sizeof(data));

	if(r.IsValid() && r.left >= 0 && r.top >= 0 &&
	   r.Width() < OLED_SCREEN_WIDTH && r.Height() < OLED_SCREEN_HEIGHT)
	{
		data.x = (uint8_t)r.left;
		data.y = (uint8_t)r.top;
		data.w = (uint8_t)(r.Width() + 1);
		data.h = (uint8_t)(r.Height() + 1);
	}

	if(ioctl(fFD, OLED_SSD1306_IOC_SET_CLIPPING, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
OLEDScreen::FillRect(BRect r,
		     pattern p,
		     bool patternVertical,
		     bigtime_t &ts)
{
	_oled_ssd1306_clear_t data;

	if(r.IsValid() == false) return B_BAD_VALUE;
	if(r.left < 0 || r.top < 0 ||
	   r.Width() >= OLED_SCREEN_WIDTH ||
	   r.Height() >= OLED_SCREEN_HEIGHT) return B_BAD_VALUE;

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

	if(ioctl(fFD, OLED_SSD1306_IOC_CLEAR, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


bool
OLEDScreen::IsFontHeightSupported(uint8 size)
{
	if(size == 8 || size == 12 || size == 14 || size == 16 || size == 24 || size == 32) return true;
	return false;
}


status_t
OLEDScreen::DrawString(const char *str,
		       BPoint pt,
		       uint8 fontHeight,
		       bool erase,
		       bigtime_t &ts)
{
	_oled_ssd1306_show_t data;

	if(str == NULL || *str == 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (int16_t)pt.x;
	data.y = (int16_t)pt.y;
	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));
	data.erase_mode = erase ? 1 : 0;

	if(ioctl(fFD, OLED_SSD1306_IOC_SHOW, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
OLEDScreen::MeasureStringWidth(const char *str,
			       uint8 fontHeight,
			       uint16 &width)
{
	_oled_ssd1306_string_width_t data;

	if(str == NULL || *str == 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));

	if(ioctl(fFD, OLED_SSD1306_IOC_STRING_WIDTH, &data) != 0) return B_ERROR;
	width = data.w;
	return B_OK;
}


status_t
OLEDScreen::InvertRect(BRect r,
		       bigtime_t &ts)
{
	_oled_ssd1306_invert_t data;

	if(r.IsValid() == false) return B_BAD_VALUE;
	if(r.left < 0 || r.top < 0 || r.Width() >= OLED_SCREEN_WIDTH || r.Height() >= OLED_SCREEN_HEIGHT) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (uint8_t)r.left;
	data.y = (uint8_t)r.top;
	data.w = (uint8_t)(r.Width() + 1);
	data.h = (uint8_t)(r.Height() + 1);

	if(ioctl(fFD, OLED_SSD1306_IOC_INVERT, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
OLEDScreen::GetPowerState(bool &state)
{
	_oled_ssd1306_power_t data;
	bzero(&data, sizeof(data));

	data.state = 2;

	if(ioctl(fFD, OLED_SSD1306_IOC_POWER, &data) != 0) return B_ERROR;
	state = (data.state == 1);
	return B_OK;
}


status_t
OLEDScreen::SetPowerState(bool state, bigtime_t &ts)
{
	_oled_ssd1306_power_t data;
	bzero(&data, sizeof(data));

	data.state = (state ? 1 : 0);
	if(ioctl(fFD, OLED_SSD1306_IOC_POWER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
OLEDScreen::GetTimestamp(bigtime_t &ts)
{
	_oled_ssd1306_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 0;

	if(ioctl(fFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
OLEDScreen::SetTimestampNow(bigtime_t &tsRet)
{
	_oled_ssd1306_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 1;

	if(ioctl(fFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	tsRet = data.ts;
	return B_OK;
}

status_t
OLEDScreen::DisableUpdate()
{
	uint8_t st = 0;

	if(ioctl(fFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


status_t
OLEDScreen::EnableUpdate()
{
	uint8_t st = 1;

	if(ioctl(fFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
uint8
OLEDScreen::Depth()
{
	return 1;
}


lbk_color_space
OLEDScreen::ColorSpace()
{
	return LBK_CS_MONO_Y;
}


status_t
OLEDScreen::SetHighColor(rgb_color c)
{
	if(c.red == 255 && c.green == 255 && c.blue == 255) return B_OK; // white
	return B_ERROR;
}


status_t
OLEDScreen::SetLowColor(rgb_color c)
{
	if(c.red == 0 && c.green == 0 && c.blue == 0) return B_OK; // black
	return B_ERROR;
}
#endif


status_t
OLEDScreen::MapBuffer(void **buf, size_t *len)
{
	EAutolock <LBPanelDevice>autolock(Panel());
	if(autolock.IsLocked() == false) return B_ERROR;

	if(buf == NULL) return B_BAD_VALUE;
	if(fBuffer != NULL) return B_ERROR;

	fBuffer = mmap(NULL,
		       OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3),
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
	*len = OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3);
	return B_OK;
}


status_t
OLEDScreen::UnmapBuffer()
{
	EAutolock <LBPanelDevice>autolock(Panel());
	if(autolock.IsLocked() == false) return B_ERROR;

	if(fBuffer == NULL) return B_BAD_VALUE;
	if(munmap(fBuffer, OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3)) != 0) return B_ERROR;
	fBuffer = NULL;
	return B_OK;
}


status_t
OLEDScreen::Flush(bigtime_t &ts)
{
	_oled_ssd1306_buffer_t data;
	bzero(&data, sizeof(data));

	data.action = 1;

	if(ioctl(fFD, OLED_SSD1306_IOC_BUFFER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


void
OLEDScreen::Sync()
{
	// Because it's not client/server mode, we don't need to sync
}


status_t
OLEDScreen::SetPowerOffTimeout(bigtime_t t)
{
	uint64_t data = (t > 0 ? (uint64_t)t : 0);

	if(ioctl(fFD, OLED_SSD1306_IOC_SET_OFF_TIMEOUT, &data) != 0) return B_ERROR;
	return B_OK;
}

