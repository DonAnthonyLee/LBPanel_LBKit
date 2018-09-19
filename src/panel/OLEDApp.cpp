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
#include <unistd.h>
#include <linux/input.h>

#include "OLEDApp.h"

#define OLED_DEBUG

#ifdef OLED_DEBUG
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif


OLEDApp::OLEDApp(int oled_fd, int input_fd)
	: fOLEDFD(oled_fd), fInputFD(input_fd),
	  fLeftSide(false), fActivatedPage(-1)
{
}


OLEDApp::~OLEDApp()
{
	OLEDView *view;

	while((view = (OLEDView*)fLeftPageViews.RemoveItem(0)) != NULL) delete view;
	while((view = (OLEDView*)fRightPageViews.RemoveItem(0)) != NULL) delete view;
}


bool
OLEDApp::AddPageView(OLEDView *view, bool left_side)
{
	if(fOLEDFD < 0 || view == NULL || view->Looper() != NULL) return false;

	if((left_side ? fLeftPageViews.AddItem(view) : fRightPageViews.AddItem(view)) == false) return false;
	AddHandler(view);

	view->fActivated = false;
	view->fFD = fOLEDFD;

	return true;
}


bool
OLEDApp::RemovePageView(OLEDView *view)
{
	if(RemoveHandler(view) == false) return false;
	if(fLeftPageViews.RemoveItem(view) == false)
		fRightPageViews.RemoveItem(view);

	// TODO: handling when it's activated one
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

	fActivatedPage = index;
	fLeftSide = left_side;

	newView->SetActivated(true);
}


OLEDView*
OLEDApp::GetActivatedPageView() const
{
	return PageViewAt(fActivatedPage, fLeftSide);
}


void
OLEDApp::Go()
{
	if(fOLEDFD < 0 || fInputFD < 0)
	{
		printf("Invalid file handle !\n");
		return;
	}

	Lock();
	Run();
	ActivatePageView(0, false);
	Unlock();

	while(IsRunning())
	{
		struct input_event event;
		int n = read(fInputFD, &event, sizeof(event));

		if(n <= 0)
		{
			perror("Unable to get event from input device");
			break;
		}

		if(n != sizeof(event))
		{
			printf("Unable to process input event !\n");
			continue;
		}

		if(event.type != EV_KEY)
		{
			DBGOUT("Event.type != EV_KEY.\n");
			continue;
		}

		BMessage msg(event.value == 0 ? B_KEY_UP : B_KEY_DOWN);
		msg.AddInt32("key", event.code);
		msg.AddInt64("when", (bigtime_t)event.time.tv_sec * (bigtime_t)(1000000) + (bigtime_t)event.time.tv_usec);
#ifdef OLED_DEBUG
		msg.PrintToStream();
#endif

		Lock();
		PostMessage(&msg);
		Unlock();
	}

	Lock();
	PostMessage(B_QUIT_REQUESTED);
	Unlock();
}

