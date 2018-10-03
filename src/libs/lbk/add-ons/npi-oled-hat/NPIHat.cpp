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
 * File: NPIHat.cpp
 * Description:
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

#include <stdint.h>
#include <oled_ssd1306_ioctl.h>

#include "Config.h"
#include "NPIHat.h"

#ifdef LBK_ENABLE_MORE_FEATURES
#include <sys/mman.h>
#endif

#if (0)
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif


extern "C" _EXPORT LBPanelDevice* instantiate_panel_device()
{
	return new NPIHat();
}


NPIHat::NPIHat()
	: LBPanelDevice(),
	  fOLEDFD(-1),
	  fInputFD(-1),
	  fThread(NULL)
{
#ifdef LBK_ENABLE_MORE_FEATURES
	fBuffer = NULL;
#endif
	fPipes[0] = -1;

	if ((fOLEDFD = open(DEFAULT_OLED_DEVICE, O_RDWR)) < 0)
	{
		perror("[NPIHat]: Unable to open OLED device !");
		return;
	}

	if ((fInputFD = open(DEFAULT_INPUT_EVENT, O_RDONLY)) < 0)
	{
		perror("[NPIHat]: Unable to open input device !");
		return;
	}

	if(pipe(fPipes) < 0)
	{
		perror("[NPIHat]: Unable to create pipe !");
		return;
	}

	// Use ETK++'s thread function directly instead of BeOS style.
	if((fThread = etk_create_thread(this->InputEventsObserver,
					E_URGENT_DISPLAY_PRIORITY,
					reinterpret_cast<void*>(this),
					NULL)) == NULL ||
	   etk_resume_thread(fThread) != E_OK)
	{
		if(fThread != NULL)
		{
			etk_delete_thread(fThread);
			fThread = NULL;
		}
		ETK_WARNING("[NPIHat]: Unable to create thread !\n");
	}
}


NPIHat::~NPIHat()
{
	// Use ETK++'s thread function directly instead of BeOS style.
	if(fThread != NULL)
	{
		if(fPipes[0] >= 0)
		{
			e_status_t status;
			uint8 t = 0xff;
			write(fPipes[1], &t, 1);
			etk_wait_for_thread(fThread, &status);
		}
		etk_delete_thread(fThread);
	}

	if(fOLEDFD >= 0) close(fOLEDFD);
	if(fInputFD >= 0) close(fInputFD);
	if(fPipes[0] >= 0)
	{
		close(fPipes[0]);
		close(fPipes[1]);
	}

#ifdef LBK_ENABLE_MORE_FEATURES
	if(fBuffer != NULL)
		munmap(fBuffer, OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3));
#endif
}


status_t
NPIHat::InitCheck(const char *options)
{
	return((fOLEDFD < 0 || fInputFD < 0 || fPipes[0] < 0 || fThread == NULL) ? B_ERROR : B_OK);
}


uint16
NPIHat::ScreenWidth()
{
	return OLED_SCREEN_WIDTH;
}


uint16
NPIHat::ScreenHeight()
{
	return OLED_SCREEN_HEIGHT;
}


status_t
NPIHat::FillRect(BRect r,
		 pattern p,
		 bool patternVertical,
		 bigtime_t &ts)
{
	_oled_ssd1306_clear_t data;

	if(r.IsValid() == false) return B_BAD_VALUE;
	if(r.left < 0 || r.top < 0 || r.Width() >= OLED_SCREEN_WIDTH || r.Height() >= OLED_SCREEN_HEIGHT) return B_BAD_VALUE;

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

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_CLEAR, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


bool
NPIHat::IsFontHeightSupported(uint8 size)
{
	if(!(size == 12 || size == 14 || size == 16 || size == 24 || size == 32)) return false;
	return true;
}


status_t
NPIHat::DrawString(const char *str,
		   BPoint pt,
		   uint8 fontHeight,
		   bool erase,
		   bigtime_t &ts)
{
	_oled_ssd1306_show_t data;

	if(str == NULL || *str == 0) return B_BAD_VALUE;
	if(pt.x < 0 || pt.y < 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.x = (uint8_t)pt.x;
	data.y = (uint8_t)pt.y;
	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));
	data.erase_mode = erase ? 1 : 0;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_SHOW, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
NPIHat::MeasureStringWidth(const char *str,
			   uint8 fontHeight,
			   uint16 &width)
{
	_oled_ssd1306_string_width_t data;

	if(str == NULL || *str == 0) return B_BAD_VALUE;

	bzero(&data, sizeof(data));

	data.size = fontHeight;
	strncpy(data.str, str, sizeof(data.str));

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_STRING_WIDTH, &data) != 0) return B_ERROR;
	width = data.w;
	return B_OK;
}


status_t
NPIHat::GetPowerState(bool &state)
{
	_oled_ssd1306_power_t data;
	bzero(&data, sizeof(data));

	data.state = 2;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_POWER, &data) != 0) return B_ERROR;
	state = (data.state == 1);
	return B_OK;
}


status_t
NPIHat::SetPowerState(bool state, bigtime_t &ts)
{
	_oled_ssd1306_power_t data;
	bzero(&data, sizeof(data));

	data.state = (state ? 1 : 0);
	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_POWER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
NPIHat::GetTimestamp(bigtime_t &ts)
{
	_oled_ssd1306_get_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 0;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


status_t
NPIHat::SetTimestampNow(bigtime_t &tsRet)
{
	_oled_ssd1306_get_ts_t data;
	bzero(&data, sizeof(data));

	data.last_action = 1;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_TIMESTAMP, &data) != 0) return B_ERROR;
	tsRet = data.ts;
	return B_OK;
}


status_t
NPIHat::DisableUpdate()
{
	uint8_t st = 0;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


status_t
NPIHat::EnableUpdate()
{
	uint8_t st = 1;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_UPDATE, &st) != 0) return B_ERROR;
	return B_OK;
}


status_t
NPIHat::GetCountOfKeys(uint8 &count)
{
	if(fInputFD < 0) return B_ERROR;
	count = OLED_KEYS_COUNT;
	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
uint8
NPIHat::ScreenDepth()
{
	return 1;
}


color_space
NPIHat::ScreenColorSpace()
{
	return (color_space)-1;
}


status_t
NPIHat::SetHighColor(rgb_color c)
{
	if(c.red == 255 && c.green == 255 && c.blue == 255) return B_OK; // white
	return B_ERROR;
}


status_t
NPIHat::SetLowColor(rgb_color c)
{
	if(c.red == 0 && c.green == 0 && c.blue == 0) return B_OK; // black
	return B_ERROR;
}
#endif


status_t
NPIHat::MapBuffer(void **buf)
{
	if(buf == NULL) return B_BAD_VALUE;
	if(fBuffer != NULL) return B_ERROR;

	fBuffer = mmap(NULL,
		       OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3),
		       PROT_READ | PROT_WRITE,
		       MAP_SHARED,
		       fOLEDFD,
		       0);
	if(fBuffer == NULL || fBuffer == MAP_FAILED)
	{
		fBuffer = NULL;
		return B_ERROR;
	}

	*buf = fBuffer;
	return B_OK;
}


status_t
NPIHat::UnmapBuffer()
{
	if(fBuffer == NULL) return B_BAD_VALUE;
	if(munmap(fBuffer, OLED_SCREEN_WIDTH * (OLED_SCREEN_HEIGHT >> 3)) != 0) return B_ERROR;
	fBuffer = NULL;
	return B_OK;
}


status_t
NPIHat::Flush(bigtime_t &ts)
{
	_oled_ssd1306_buffer_t data;
	bzero(&data, sizeof(data));

	data.action = 1;

	if(ioctl(fOLEDFD, OLED_SSD1306_IOC_BUFFER, &data) != 0) return B_ERROR;
	ts = data.ts;
	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
status_t
NPIHat::GetOrientationOfKeys(orientation &o)
{
	o = B_HORIZONTAL;
	return B_OK;
}


status_t
NPIHat::GetSideOfKeys(bool &right_or_bottom)
{
	right_or_bottom = true;
	return B_OK;
}


status_t
NPIHat::GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
			      uint16 &offsetRightBottom)
{
	offsetLeftTop = offsetRightBottom = 0;
	return B_OK;
}
#endif


int
NPIHat::InputHandle() const
{
	return fInputFD;
}


int
NPIHat::PipeHandle() const
{
	return fPipes[0];
}


int32
NPIHat::InputEventsObserver(void *arg)
{
	NPIHat *self = reinterpret_cast<NPIHat*>(arg);

	fd_set rset;
	int input_fd = self->InputHandle();
	int pipe_fd = self->PipeHandle();
	bool doingGood = (input_fd < 0 || pipe_fd < 0) ? false : true;

	while(doingGood)
	{
		FD_ZERO(&rset);
		FD_SET(input_fd, &rset);
		FD_SET(pipe_fd, &rset);

		int status = select(max_c(input_fd, pipe_fd) + 1, &rset, NULL, NULL, NULL);
		if(status < 0)
		{
			perror("[NPIHat]: Unable to get event from input device");
			break;
		}

		if(status > 0 && FD_ISSET(pipe_fd, &rset))
		{
			uint8 byte = 0x00;
			doingGood = !(read(pipe_fd, &byte, 1) != 1 || byte == 0xff);
		}

		if(status == 0) continue;

		if(!FD_ISSET(input_fd, &rset)) continue;

		struct input_event event;
		int n = read(input_fd, &event, sizeof(event));

		if(n <= 0)
		{
			perror("[NPIHat]: Unable to get event from input device");
			break;
		}

		if(n != sizeof(event))
		{
			printf("[NPIHat]: Unable to process input event !\n");
			continue;
		}

		if(event.type != EV_KEY)
		{
			DBGOUT("[NPIHat]: event.type(%u) != EV_KEY.\n", event.type);
			continue;
		}

		uint8 nKey;
		if(event.code == OLED_KEY1_CODE) nKey = 0;
#if OLED_KEYS_COUNT > 1
		else if(event.code == OLED_KEY2_CODE) nKey = 1;
#endif
#if OLED_KEYS_COUNT > 2
		else if(event.code == OLED_KEY3_CODE) nKey = 2;
#endif
#if OLED_KEYS_COUNT > 3
		else if(event.code == OLED_KEY4_CODE) nKey = 3;
#endif
#if OLED_KEYS_COUNT > 4
		else if(event.code == OLED_KEY5_CODE) nKey = 4;
#endif
#if OLED_KEYS_COUNT > 5
		else if(event.code == OLED_KEY6_CODE) nKey = 5;
#endif
#if OLED_KEYS_COUNT > 6
		else if(event.code == OLED_KEY7_CODE) nKey = 6;
#endif
#if OLED_KEYS_COUNT > 7
		else if(event.code == OLED_KEY8_CODE) nKey = 7;
#endif
		else continue;

		BMessage msg(event.value == 0 ? B_KEY_UP : B_KEY_DOWN);
		msg.AddInt8("key", *((int8*)&nKey));
		msg.AddInt64("when", (bigtime_t)event.time.tv_sec * (bigtime_t)(1000000) + (bigtime_t)event.time.tv_usec);

		self->SendMessageToApp(&msg);
	}

	self->SendMessageToApp(B_QUIT_REQUESTED);
	return 0;
}

