/* --------------------------------------------------------------------------
 *
 * Simple MPD client using LBKit
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
 * File: TMainPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "IconDefs.h"
#include "TMainPageView.h"

TMainPageView::TMainPageView(const char *name)
	: LBPageView(name)
{
	// TODO
	SetNavButtonIcon(0, LBK_ICON_ID_16x16_USER1);
	SetNavButtonIcon(1, LBK_ICON_ID_16x16_USER2);
	SetNavButtonIcon(2, LBK_ICON_ID_16x16_USER3);

	ShowNavButton(0);
	ShowNavButton(1);
	ShowNavButton(2);

#if 0
	if(fClient.Connect("localhost", 6600) == B_OK)
	{
		uint8 a, b, c;
		if(fClient.GetMPDVersion(&a, &b, &c) != NULL)
			ETK_WARNING("MPD Version = %u.%u.%u", a, b, c);
		else
			ETK_WARNING("GetMPDVersion() failed");

		BMessage msg;
		if(fClient.GetStatus(&msg) == B_OK) msg.PrintToStream();
	}
	else
	{
		ETK_WARNING("Connect() failed");
	}
#endif
}


TMainPageView::~TMainPageView()
{
	// TODO
}


void
TMainPageView::Pulse()
{
	// TODO
}


void
TMainPageView::Draw(BRect updateRect)
{
	// TODO
	LBPageView::Draw(updateRect);
}


void
TMainPageView::KeyDown(uint8 key, uint8 clicks)
{
	// TODO
	LBPageView::KeyDown(key, clicks);
}


void
TMainPageView::KeyUp(uint8 key, uint8 clicks)
{
	// TODO
	LBPageView::KeyUp(key, clicks);
}


void
TMainPageView::Activated(bool state)
{
	// TODO
	LBPageView::Activated(state);
}


void
TMainPageView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		// TODO
		default:
			LBPageView::MessageReceived(msg);
	}
}


void
TMainPageView::DrawNavButtonIcon(lbk_icon_id idIcon, BPoint location)
{
	// TODO
	switch(idIcon)
	{
		case LBK_ICON_ID_16x16_USER1:
			DrawIcon(&icon_prev, location);
			break;

		case LBK_ICON_ID_16x16_USER2:
			DrawIcon(&icon_play, location);
			break;

		case LBK_ICON_ID_16x16_USER3:
			DrawIcon(&icon_next, location);
			break;

		default:
			DrawIcon(idIcon, location);
	}
}

