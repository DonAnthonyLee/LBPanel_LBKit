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
 * File: LBPanelScreen.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelScreen.h>


LBPanelScreen::LBPanelScreen()
	: LBPanelDeviceAddOn(),
	  fLocation(B_ORIGIN)
{
	// TODO
}


LBPanelScreen::LBPanelScreen(BPoint location)
	: LBPanelDeviceAddOn(),
	  fLocation(location)
{
	// TODO
}


LBPanelScreen::~LBPanelScreen()
{
	// TODO
}


status_t
LBPanelScreen::SendMessage(const BMessage *msg)
{
	if(msg == NULL) return B_BAD_VALUE;

	BMessage aMsg(*msg);
#ifdef ETK_MAJOR_VERSION
	aMsg.RemoveData("screen_id");
#else
	aMsg.RemoveName("screen_id");
#endif
	aMsg.AddInt32("screen_id", Index());

	return LBPanelDeviceAddOn::SendMessage(&aMsg);
}


BPoint
LBPanelScreen::Location() const
{
	return fLocation;
}

