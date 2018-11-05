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
 * File: VPD.cpp
 * Description: Virtual panel device
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>

#include <lbk/LBApplication.h> // for "LBK_QUIT_REQUESTED"

#include "VPDApp.h"
#include "VPD.h"

#ifdef ETK_MAJOR_VERSION
	#define BScreen			EScreen
#endif


extern "C" _EXPORT LBPanelDevice* instantiate_panel_device()
{
	return new LBVPD();
}


LBVPD::LBVPD()
	: LBPanelDevice(),
	  fThread(NULL),
	  fWidth(128),
	  fHeight(64),
	  fKeysCount(3),
	  fState(true),
	  fTimestamp(0),
	  fBuffer(NULL),
	  fBufferLen(0)
{
#ifdef LBK_ENABLE_MORE_FEATURES
	fDepth = 1;
	fColorSpace = (color_space)-1;
	fOrientation = B_HORIZONTAL;
	fKeysRB = true;
	fKeysOffset[0] = fKeysOffset[1] = 0;
#endif

	if(be_app != NULL) return;

#ifdef ETK_MAJOR_VERSION
	if((fThread = etk_create_thread(this->RunBeApp,
					E_URGENT_DISPLAY_PRIORITY,
					NULL,
					NULL)) == NULL ||
	   etk_resume_thread(fThread) != E_OK)
	{
		if(fThread != NULL)
		{
			etk_delete_thread(fThread);
			fThread = NULL;
		}
		ETK_WARNING("[VPD]: Unable to create thread !\n");
	}
#else

	thread_id tid = spawn_thread(this->RunBeApp, "lbk_vpd_app", B_NORMAL_PRIORITY, NULL);
	if(tid < 0 || resume_thread(tid) != B_OK)
	{
		fprintf(stderr, "[VPD]: Unable to spawn thread !\n");
	}
	else
	{
		fThread = reinterpret_cast<void*>((int)tid);
	}
#endif
}


LBVPD::~LBVPD()
{
	if(fMsgr.LockTarget() == B_OK)
	{
		BLooper *looper = NULL;

		fMsgr.Target(&looper);
		if(looper != NULL)
			looper->Quit();
	}

	if(fBuffer != NULL)
		free(fBuffer);

#if 0
	/*
	 * Note: WHY DID WE DISABLE THE FOLLOWING CODES ?
	 * 	"BApplication" will try to quit all loopers when quitting,
	 *	including the "LBApplication" which derived from "BLooper".
	 * 	So, just leave it alone even knowing it's a bad idea, but,
	 * 	it's just a virtual device, who cares ?
	 */
#ifdef ETK_MAJOR_VERSION
	if(fThread != NULL)
	{
		if(be_app != NULL)
			be_app->PostMessage(B_QUIT_REQUESTED);

		e_status_t status;
		etk_wait_for_thread(fThread, &status);

		etk_delete_thread(fThread);
	}
#else
	thread_id tid = (fThread == NULL ? 0 : (thread_id)reinterpret_cast<int>(fThread));
	if(tid > 0 && be_app != NULL)
	{
		be_app->PostMessage(B_QUIT_REQUESTED);

		status_t status;
		wait_for_thread(tid, &status);
	}
#endif
#endif
}


status_t
LBVPD::InitCheck(const char *options)
{
	snooze(200000); // wait for be_app

	if(be_app == NULL || fMsgr.IsValid()) return B_ERROR;
#ifdef ETK_MAJOR_VERSIN
	while(be_app->IsRunning() == false) snooze(100000);
#else
	while(be_app->Thread() < 0) snooze(100000);
#endif

	BString opt(options);
	BPoint pt(100, 100);
	BString title("Virutal Panel Device for LBKit");
	int32 id = -1;
	uint8 point_size = 1;
	BScreen screen;
	lbk_color_space cspace = LBK_CS_MONO_Y;

	opt.ReplaceAll(",", " ");
	while(opt.FindFirst("  ") >= 0) opt.ReplaceAll("  ", " ");
	while(opt.Length() > 0)
	{
		int32 found = opt.FindFirst(' ');
		if(found < 0) found = opt.Length();

		BString item(opt, found);
		opt.Remove(0, found + 1);

		if(item.Length() <= 0) continue;

		BString value;
		found = item.FindFirst('=');
		if(found >= 0)
		{
			value.SetTo(item.String() + found + 1);
			item.Truncate(found);
		}

#if 0
		fprintf(stdout, "[VPD]: item=\"%s\", value=\"%s\"\n", item.String(), value.String());
#endif

		if(item == "width")
		{
			int v = atoi(value.String());
			if(v > 0 && v <= 2048) fWidth = (uint16)v;
		}
		else if(item == "height")
		{
			int v = atoi(value.String());
			if(v > 0 && v <= 2048) fHeight = (uint16)v;
		}
		else if(item == "point_size")
		{
			int v = atoi(value.String());
			if(v > 0 && v <= 255) point_size = (uint8)v;
		}
		else if(item == "keys_count")
		{
			int v = atoi(value.String());
			if(v >= 0 && v <= 8) fKeysCount = (uint8)v;
		}
		else if(item == "x")
		{
			int v = atoi(value.String());
			if(v >= 0 && v <= (int)screen.Frame().Width()) pt.x = v;
		}
		else if(item == "y")
		{
			int v = atoi(value.String());
			if(v >= 0 && v <= (int)screen.Frame().Height()) pt.y = v;
		}
		else if(item == "title")
		{
			title.Truncate(0);
			title.SetTo(value);
			title.ReplaceAll("&nbsp;", " ");
		}
		else if(item == "id")
		{
			id = atoi(value.String());
		}
#ifdef LBK_ENABLE_MORE_FEATURES
		else if(item == "depth")
		{
			int v = atoi(value.String());
			if(v == 8 || v == 16 || v == 24 || v == 32) fDepth = (uint8)v;
		}
		else if(item == "color_space")
		{
			// TODO
		}
		else if(item == "keys_RB")
		{
			fKeysRB = (value == "1" || value.ICompare("true") == 0);
		}
		else if(item == "orient")
		{
			fOrientation = (value == "0" || value.ICompare("horizontal") == 0) ? B_HORIZONTAL : B_VERTICAL;
		}
		else if(item == "offset_LT")
		{
			int v = atoi(value.String());
			if(v >= 0 && v < 65535) fKeysOffset[0] = (uint16)v;
		}
		else if(item == "offset_RB")
		{
			int v = atoi(value.String());
			if(v >= 0 && v < 65535) fKeysOffset[1] = (uint16)v;
		}
#endif
	}

	BRect r(pt, pt + BPoint(200, 200));
	VPDWindow *win = new VPDWindow(r, title.String(), fWidth, fHeight, fKeysCount, point_size, id, cspace, this);

	win->Lock();
	win->Show();
	fMsgr = BMessenger(win, win);
	win->Unlock();

	return B_OK;
}


uint16
LBVPD::ScreenWidth()
{
	return fWidth;
}


uint16
LBVPD::ScreenHeight()
{
	return fHeight;
}


#ifdef LBK_ENABLE_MORE_FEATURES
uint8
LBVPD::ScreenDepth()
{
	return fDepth;
}


color_space
LBVPD::ScreenColorSpace()
{
	return fColorSpace;
}


status_t
LBVPD::SetHighColor(rgb_color c)
{
	// TODO
	return B_ERROR;
}


status_t
LBVPD::SetLowColor(rgb_color c)
{
	// TODO
	return B_ERROR;
}
#endif


status_t
LBVPD::FillRect(BRect rect,
		pattern p,
		bool patternVertical,
		bigtime_t &ts)
{
	BMessage msg(VPD_MSG_FILL_RECT);
	msg.AddRect("rect", rect);
	msg.AddInt64("pattern", *((int64*)&p));
	msg.AddBool("vpattern", patternVertical);

	status_t st = fMsgr.SendMessage(&msg);
	if(st == B_OK)
	{
		Lock();
		ts = fTimestamp = real_time_clock_usecs();
		Unlock();
	}

	return st;
}


bool
LBVPD::IsFontHeightSupported(uint8 fontHeight)
{
	BMessage replyMsg;
	BMessage msg(VPD_MSG_STRING_WIDTH);
	msg.AddString("string", "A");
	msg.AddInt8("height", *((int8*)&fontHeight));

	status_t st = fMsgr.SendMessage(&msg, &replyMsg);
	if(st != B_OK) return false;

	uint16 w = 0;

	if(replyMsg.what != B_REPLY ||
	   replyMsg.FindInt16("width", (int16*)&w) != B_OK ||
	   w == 0)
		return false;

	return true;
}


status_t
LBVPD::DrawString(const char *str,
		  BPoint pt,
		  uint8 fontHeight,
		  bool erase,
		  bigtime_t &ts)
{
	BMessage msg(VPD_MSG_DRAW_STRING);
	msg.AddString("string", str);
	msg.AddPoint("location", pt);
	msg.AddInt8("height", *((int8*)&fontHeight));
	msg.AddBool("erase_mode", erase);

	status_t st = fMsgr.SendMessage(&msg);
	if(st == B_OK)
	{
		Lock();
		ts = fTimestamp = real_time_clock_usecs();
		Unlock();
	}

	return st;
}


status_t
LBVPD::MeasureStringWidth(const char *str,
			  uint8 fontHeight,
			  uint16 &width)
{
	BMessage replyMsg;
	BMessage msg(VPD_MSG_STRING_WIDTH);
	msg.AddString("string", str);
	msg.AddInt8("height", *((int8*)&fontHeight));

	status_t st = fMsgr.SendMessage(&msg, &replyMsg);
	if(st != B_OK) return st;

	if(replyMsg.what != B_REPLY ||
	   replyMsg.FindInt16("width", (int16*)&width) != B_OK)
		return B_ERROR;

	return B_OK;
}


status_t
LBVPD::InvertRect(BRect rect,
		  bigtime_t &ts)
{
	BMessage msg(VPD_MSG_INVERT_RECT);
	msg.AddRect("rect", rect);

	status_t st = fMsgr.SendMessage(&msg);
	if(st == B_OK)
	{
		Lock();
		ts = fTimestamp = real_time_clock_usecs();
		Unlock();
	}

	return st;
}


status_t
LBVPD::GetPowerState(bool &state)
{
	state = fState;

	return B_OK;
}


status_t
LBVPD::SetPowerState(bool state,
		     bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBVPD>autolock(this);
#else
	BAutolock autolock(this);
#endif

	if(state != fState)
	{
		BMessage msg(VPD_MSG_POWER_STATE);
		msg.AddBool("state", state);

		status_t st = fMsgr.SendMessage(&msg);
		if(st != B_OK) return st;
		ts = fTimestamp = real_time_clock_usecs();

		// store the state to local var for "GetPowerState()".
		fState = state;
	}

	return B_OK;
}


status_t
LBVPD::GetTimestamp(bigtime_t &ts)
{
	ts = fTimestamp;

	return B_OK;
}


status_t
LBVPD::SetTimestampNow(bigtime_t &tsRet)
{
	Lock();
	tsRet = fTimestamp = real_time_clock_usecs();
	Unlock();

	return B_OK;
}


status_t
LBVPD::DisableUpdate()
{
	BMessage msg(VPD_MSG_ENABLE_UPDATE);
	msg.AddBool("update", false);

	return fMsgr.SendMessage(&msg);
}


status_t
LBVPD::EnableUpdate()
{
	BMessage msg(VPD_MSG_ENABLE_UPDATE);
	msg.AddBool("update", true);

	return fMsgr.SendMessage(&msg);
}


#ifdef ETK_MAJOR_VERSION
	#define FindData		BFindData
	#define B_POINTER_TYPE		E_POINTER_TYPE
#endif

status_t
LBVPD::MapBuffer(void **buf, size_t *len)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBVPD>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fBuffer != NULL) return B_ERROR;

	BMessage replyMsg;
	BMessage msg(VPD_MSG_GET_BUFFER);

	status_t st = fMsgr.SendMessage(&msg, &replyMsg);
	if(st != B_OK) return st;

	const void *data = NULL;
	ssize_t nBytes = 0;

	if(replyMsg.what != B_REPLY ||
	   replyMsg.FindData("buffer", B_POINTER_TYPE, &data, &nBytes) != B_OK ||
#ifdef LBK_ENABLE_MORE_FEATURES
	   nBytes < (ssize_t)(((uint32)fWidth * (uint32)fHeight * (uint32)fDepth) >> 3) ||
#else
	   nBytes < (ssize_t)(((uint32)fWidth * (uint32)fHeight) >> 3) ||
#endif
	   data == NULL)
		return B_ERROR;

	if((fBuffer = malloc(nBytes)) == NULL)
		return B_ERROR;

	memcpy(fBuffer, data, nBytes);
	fBufferLen = (size_t)nBytes;

	*buf = fBuffer;
	*len = fBufferLen;
	return B_OK;
}

#ifdef ETK_MAJOR_VERSION
	#undef FindData
#endif


status_t
LBVPD::UnmapBuffer()
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBVPD>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fBuffer == NULL) return B_ERROR;

	free(fBuffer);
	fBuffer = NULL;

	return B_OK;
}


status_t
LBVPD::Flush(bigtime_t &ts)
{
#ifdef ETK_MAJOR_VERSION
	EAutolock <LBVPD>autolock(this);
#else
	BAutolock autolock(this);
#endif
	if(autolock.IsLocked() == false || fBuffer == NULL) return B_ERROR;

	BMessage replyMsg;
	BMessage msg(VPD_MSG_SET_BUFFER);
	msg.AddData("buffer", B_POINTER_TYPE, fBuffer, fBufferLen, true);

	status_t st = fMsgr.SendMessage(&msg, &replyMsg);
	if(st != B_OK) return st;

	return(replyMsg.what != B_REPLY ? B_ERROR : B_OK);
}


void
LBVPD::Sync()
{
	BMessage replyMsg;
	BMessage msg(VPD_MSG_SYNC);

	fMsgr.SendMessage(&msg, &replyMsg);
}


status_t
LBVPD::GetCountOfKeys(uint8 &count)
{
	count = fKeysCount;

	return B_OK;
}


#ifdef LBK_ENABLE_MORE_FEATURES
status_t
LBVPD::GetOrientationOfKeys(orientation &o)
{
	o = fOrientation;

	return B_OK;
}


status_t
LBVPD::GetSideOfKeys(bool &right_or_bottom)
{
	right_or_bottom = fKeysRB;

	return B_OK;
}


status_t
LBVPD::GetScreenOffsetOfKeys(uint16 &offsetLeftTop,
			     uint16 &OffsetRightBottom)
{
	offsetLeftTop = fKeysOffset[0];
	offsetRightBottom = fKeysOffset[1];

	if(offsetLeftTop > ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight))
		offsetLeftTop = ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight);

	if(offsetRightBottom > ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight) - offsetLeftTop)
		offsetRightBottom = ((fOrientation == B_HORIZONTAL) ? fWidth : fHeight) - offsetLeftTop;

	return B_OK;
}
#endif


int32
LBVPD::RunBeApp(void *arg)
{
	VPDApplication *app = new VPDApplication();

	app->Run();

	// NOTE: "delete app" crash on HaikuOS
	app->Lock();
	app->Quit();

	return 0;
}


void
LBVPD::KeyDown(uint8 key, bigtime_t when)
{
	BMessage msg(B_KEY_DOWN);
	msg.AddInt8("key", *((int8*)&key));
	msg.AddInt64("when", when);

	SendMessageToApp(&msg);
}


void
LBVPD::KeyUp(uint8 key, bigtime_t when)
{
	BMessage msg(B_KEY_UP);
	msg.AddInt8("key", *((int8*)&key));
	msg.AddInt64("when", when);

	SendMessageToApp(&msg);
}


void
LBVPD::QuitRequested()
{
	SendMessageToApp(LBK_QUIT_REQUESTED);
}

