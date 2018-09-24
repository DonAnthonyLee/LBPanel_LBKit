/* --------------------------------------------------------------------------
 *
 * Panel Application for NanoPi OLED Hat
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
 * File: OLEDApp.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>

#include "OLEDConfig.h"
#include <OLEDApp.h>

//#define OLED_DEBUG

#ifdef OLED_DEBUG
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif


OLEDApp::OLEDApp(int oled_fd, int input_fd)
	: BLooper(NULL, B_DISPLAY_PRIORITY),
	  fOLEDFD(oled_fd), fInputFD(input_fd),
	  fPulseRate(0),
	  fKeyState(0)
{
	bzero(fKeyTimestamps, sizeof(fKeyTimestamps));
	bzero(fKeyClicks, sizeof(fKeyClicks));

	fPipes[0] = fPipes[1] = -1;
}


OLEDApp::~OLEDApp()
{
	OLEDView *view;
	while((view = (OLEDView*)fLeftPageViews.RemoveItem(0)) != NULL) delete view;
	while((view = (OLEDView*)fRightPageViews.RemoveItem(0)) != NULL) delete view;

	if(fPipes[0] >= 0) close(fPipes[0]);
	if(fPipes[1] >= 0) close(fPipes[1]);
}


bool
OLEDApp::AddPageView(OLEDView *view, bool left_side)
{
	if(fOLEDFD < 0 || view == NULL || view->Looper() != NULL || view->MasterView() != NULL) return false;

	if((left_side ? fLeftPageViews.AddItem(view) : fRightPageViews.AddItem(view)) == false) return false;
	AddHandler(view);

	view->fActivated = false;
	view->fFD = fOLEDFD;

	view->Attached();

	return true;
}


bool
OLEDApp::RemovePageView(OLEDView *view)
{
	if(view == NULL || view->Looper() != this || view->MasterView() != NULL) return false;

	view->Detached();
	RemoveHandler(view);

	if(fLeftPageViews.RemoveItem(view) == false)
		fRightPageViews.RemoveItem(view);

	if(view->fActivated)
		SetPreferredHandler(NULL);

	view->fActivated = false;
	view->fFD = -1;

	return true;
}


OLEDView*
OLEDApp::RemovePageView(int32 index, bool left_side)
{
	OLEDView *view = PageViewAt(index, left_side);

	return(RemovePageView(view) ? view : NULL);
}


OLEDView*
OLEDApp::PageViewAt(int32 index, bool left_side) const
{
	return(left_side ? (OLEDView*)fLeftPageViews.ItemAt(index) : (OLEDView*)fRightPageViews.ItemAt(index));
}


int32
OLEDApp::CountPageViews(bool left_side) const
{
	return(left_side ? fLeftPageViews.CountItems() : fRightPageViews.CountItems());
}

void
OLEDApp::ActivatePageView(int32 index, bool left_side)
{
	OLEDView *newView = PageViewAt(index, left_side);
	OLEDView *oldView = GetActivatedPageView();

	if(oldView == newView || newView == NULL) return;

	if(oldView)
		oldView->SetActivated(false);

	SetPreferredHandler(newView);
	newView->SetActivated(true);
}


OLEDView*
OLEDApp::GetActivatedPageView() const
{
	// ignore NULL to make it compatible with old BeOS API
	return(PreferredHandler() ? cast_as(PreferredHandler(), OLEDView) : NULL);
}


void
OLEDApp::Go()
{
	if(IsRunning())
	{
		printf("[OLEDApp]: It's forbidden to run Go() more than ONE time !\n");
		return;
	}

	if(fOLEDFD < 0 || fInputFD < 0)
	{
		printf("[OLEDApp]: Invalid file handle !\n");
		return;
	}

	if(pipe(fPipes) < 0)
	{
		perror("[OLEDApp]: Unable to create pipe");
		return;
	}

	Lock();
	Run();
	ActivatePageView(0, false);
	Unlock();

	fd_set rset;
	struct timeval timeout;
	uint32 count = 0;
	bigtime_t pulse_sent_time[2] = {0, 0};
	bigtime_t pulse_rate = 0;

	timeout.tv_sec = 0;
	while(IsRunning())
	{
		timeout.tv_usec = (pulse_rate > 0 && pulse_rate < (bigtime_t)500000) ? pulse_rate : 500000;
		if(count > 0 && timeout.tv_usec > OLED_BUTTON_INTERVAL / 2)
			timeout.tv_usec = OLED_BUTTON_INTERVAL / 2;

		FD_ZERO(&rset);
		FD_SET(fInputFD, &rset);
		FD_SET(fPipes[0], &rset);
		int status = select(max_c(fInputFD, fPipes[0]) + 1, &rset, NULL, NULL, &timeout);
		if(status < 0)
		{
			perror("[OLEDApp]: Unable to get event from input device");
			break;
		}

		if(status > 0 && FD_ISSET(fPipes[0], &rset))
		{
			uint8 byte = 0x00;
			if(read(fPipes[0], &byte, 1) == 1)
			{
				switch(byte)
				{
					case 0xab:
						count++;
						break;

					default:
						pulse_rate = this->PulseRate();
				}
			}
		}

		if(count > 0 && real_time_clock_usecs() - pulse_sent_time[0] >= (bigtime_t)(OLED_BUTTON_INTERVAL / 2))
		{
			count--;
			Lock();
			PostMessage(B_PULSE, this);
			Unlock();
			pulse_sent_time[0] = real_time_clock_usecs();
		}

		if(pulse_rate > 0 && real_time_clock_usecs() - pulse_sent_time[1] >= pulse_rate)
		{
			BMessage aMsg(B_PULSE);
			aMsg.AddBool("no_button_check", true);

			Lock();
			PostMessage(&aMsg, this);
			Unlock();
			pulse_sent_time[1] = real_time_clock_usecs();
		}

		if(status == 0) continue;

		if(!FD_ISSET(fInputFD, &rset)) continue;

		struct input_event event;
		int n = read(fInputFD, &event, sizeof(event));

		if(n <= 0)
		{
			perror("[OLEDApp]: Unable to get event from input device");
			break;
		}

		if(n != sizeof(event))
		{
			printf("[OLEDApp]: Unable to process input event !\n");
			continue;
		}

		if(event.type != EV_KEY)
		{
			DBGOUT("[OLEDAPP]: event.type(%u) != EV_KEY.\n", event.type);
			continue;
		}

		BMessage msg(event.value == 0 ? B_KEY_UP : B_KEY_DOWN);
		msg.AddInt32("key", event.code);
		msg.AddInt64("when", (bigtime_t)event.time.tv_sec * (bigtime_t)(1000000) + (bigtime_t)event.time.tv_usec);

		Lock();
		PostMessage(&msg, this);
		Unlock();
	}

	Lock();
	PostMessage(B_QUIT_REQUESTED, this);
	Unlock();
}


void
OLEDApp::MessageReceived(BMessage *msg)
{
	int32 key;
	bigtime_t when;
	uint8 nKey;
	bool stopRunner;

	switch(msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
			if(PreferredHandler() == NULL) break;
			if(msg->FindInt32("key", &key) != B_OK) break;
			if(msg->FindInt64("when", &when) != B_OK) break;
			if(key == OLED_BUTTON1) nKey = 0;
#if OLED_BUTTONS_NUM > 1
			else if(key == OLED_BUTTON2) nKey = 1;
#endif
#if OLED_BUTTONS_NUM > 2
			else if(key == OLED_BUTTON3) nKey = 2;
#endif
#if OLED_BUTTONS_NUM > 3
			else if(key == OLED_BUTTON4) nKey = 3;
#endif
#if OLED_BUTTONS_NUM > 4
			else if(key == OLED_BUTTON5) nKey = 4;
#endif
#if OLED_BUTTONS_NUM > 5
			else if(key == OLED_BUTTON6) nKey = 5;
#endif
#if OLED_BUTTONS_NUM > 6
			else if(key == OLED_BUTTON7) nKey = 6;
#endif
#if OLED_BUTTONS_NUM > 7
			else if(key == OLED_BUTTON8) nKey = 7;
#endif
			else break;
			if(msg->what == B_KEY_DOWN)
			{
				if((fKeyState & (0x01 << nKey)) != 0) // already DOWN
				{
					// auto-repeat (event.code = 2) event
					if(when < fKeyTimestamps[nKey]) break;
					if(when - fKeyTimestamps[nKey] < (bigtime_t)3000000) break; // 3s
					if(fKeyClicks[nKey] == 0xff) break;
					fKeyClicks[nKey] = 0xff; // long press
				}
				else
				{
					if(fKeyClicks[nKey] > 0 && when < fKeyTimestamps[nKey]) break;
					if(fKeyClicks[nKey] < 0xff) fKeyClicks[nKey]++;
				}
				fKeyTimestamps[nKey] = when;

				BMessage aMsg(B_KEY_DOWN);
				aMsg.AddInt8("key", *((int8*)&nKey));
				aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[nKey]));
				aMsg.AddInt64("when", when);
				PostMessage(&aMsg, PreferredHandler());

				fKeyState |= (0x1 << nKey);
			}
			else
			{
				uint8 byte = 0xab;

				if((fKeyState & (0x01 << nKey)) == 0) break; // already UP
				if(when < fKeyTimestamps[nKey]) break;
				if(write(fPipes[1], &byte, 1) <= 0)
				{
					DBGOUT("[OLEDApp]: Failed to notice the main thread.\n");
					BMessage aMsg(B_KEY_UP);
					aMsg.AddInt8("key", *((int8*)&nKey));
					aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[nKey]));
					aMsg.AddInt64("when", when);
					PostMessage(&aMsg, PreferredHandler());

					fKeyState &= ~(0x01 << nKey);
					fKeyClicks[nKey] = 0;
					fKeyTimestamps[nKey] = 0;
				}
				else
				{
					fKeyState &= ~(0x01 << nKey);
					fKeyTimestamps[nKey] = when;
				}
			}
			break;

		case B_PULSE:
			DBGOUT("[OLEDApp]: B_PULSE received.\n");
			if(msg->HasBool("no_button_check"))
			{
				if(PreferredHandler() != NULL)
					PostMessage(B_PULSE, PreferredHandler());
				break;
			}
			stopRunner = true;
			when = real_time_clock_usecs();
			for(uint8 k = 0; k < OLED_BUTTONS_NUM; k++)
			{
				if((fKeyState & (0x01 << k)) != 0) continue; // DOWN, no need
				if(fKeyClicks[k] == 0 || fKeyTimestamps[k] == 0) continue; // no UP before
				if(when < fKeyTimestamps[k]) continue; // should never happen
				if(when - fKeyTimestamps[k] < (bigtime_t)OLED_BUTTON_INTERVAL)
				{
					stopRunner = false;
					continue;
				}

				if(PreferredHandler() != NULL)
				{
					BMessage aMsg(B_KEY_UP);
					aMsg.AddInt8("key", *((int8*)&k));
					aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[k]));
					aMsg.AddInt64("when", fKeyTimestamps[k]);
					PostMessage(&aMsg, PreferredHandler());
				}

				fKeyState &= ~(0x01 << k);
				fKeyClicks[k] = 0;
				fKeyTimestamps[k] = 0;
			}
			if(stopRunner == false)
			{
				uint8 byte = 0xab;
				if(write(fPipes[1], &byte, 1) <= 0)
				{
					DBGOUT("[OLEDApp]: Failed to notice the main thread.\n");
					PostMessage(B_PULSE, this); // try again
				}
			}
			break;

		default:
			BLooper::MessageReceived(msg);
	}
}


bigtime_t
OLEDApp::PulseRate() const
{
	return fPulseRate;
}


void
OLEDApp::SetPulseRate(bigtime_t rate)
{
	if(rate != 0 && rate < (bigtime_t)50000)
		rate = (bigtime_t)50000;
	else if(rate > (bigtime_t)10000000)
		rate = (bigtime_t)10000000;

	if(fPulseRate != rate)
	{
		fPulseRate = rate;

		if(fPipes[1] >= 0)
		{
			uint8 byte = 0x01;
			write(fPipes[1], &byte, 1);
		}
	}
}

