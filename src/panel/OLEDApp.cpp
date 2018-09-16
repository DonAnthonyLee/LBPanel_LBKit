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

#include "OLEDApp.h"


OLEDApp::OLEDApp(int oled_fd, int input_fd)
	: fOLEDFD(oled_fd), fInputFD(input_fd)
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
	if(fOLEDFD < 0 || view->fFD >= 0) return false;

	if((left_side ? fLeftPageViews.AddItem(view) : fRightPageViews.AddItem(view)) == false) return false;

	// TODO: activate, etc.
	view->fFD = fOLEDFD;

	return true;
}


void
OLEDApp::RemovePageView(OLEDView *view)
{
	if(view->fFD < 0 || view->fFD != fOLEDFD) return;

	if(fLeftPageViews.RemoveItem(view) == false)
		fRightPageViews.RemoveItem(view);

	// TODO: activate, etc.
	view->fActivated = false;
	view->fFD = -1;
}


OLEDView*
OLEDApp::RemovePageView(int32 index, bool left_side)
{
	OLEDView *view;

	view = (left_side ? (OLEDView*)fLeftPageViews.RemoveItem(index) : (OLEDView*)fRightPageViews.RemoveItem(index));

	// TODO: activate, etc.
	view->fActivated = false;
	view->fFD = -1;

	return view;
}


OLEDView*
OLEDApp::PageViewAt(int32 index, bool left_side)
{
	return(left_side ? (OLEDView*)fLeftPageViews.ItemAt(index) : (OLEDView*)fRightPageViews.ItemAt(index));
}


int32
OLEDApp::CountPageViews(bool left_side)
{
	return(left_side ? fLeftPageViews.CountItems() : fRightPageViews.CountItems());
}

void
OLEDApp::ActivatePageView(int32 index, bool left_side)
{
	// TODO
}


bool
OLEDApp::Lock()
{
	return fLocker.Lock();
}


void
OLEDApp::Unlock()
{
	fLocker.Unlock();
}


status_t
OLEDApp::LockWithTimeout(bigtime_t microseconds)
{
	return fLocker.LockWithTimeout(microseconds);
}


void
OLEDApp::Run()
{
	// TODO
}

