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
 * File: LBPanelDeviceAddOn.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelDeviceAddOn.h>
#include <lbk/add-ons/LBPanelDevice.h>


LBPanelDeviceAddOn::LBPanelDeviceAddOn()
	: fID(-1), fDev(NULL), fAddOn(NULL)
{
	// TODO
}


LBPanelDeviceAddOn::~LBPanelDeviceAddOn()
{
	// TODO
}


status_t
LBPanelDeviceAddOn::SendMessage(const BMessage *msg)
{
	if(msg == NULL || fID < 0 || fDev == NULL) return B_BAD_VALUE;

	return fMsgr.SendMessage(msg);
}


status_t
LBPanelDeviceAddOn::SendMessage(uint32 command)
{
	BMessage msg(command);
	return SendMessage(&msg);
}


int32
LBPanelDeviceAddOn::Index() const
{
	return fID;
}


LBPanelDevice*
LBPanelDeviceAddOn::Panel() const
{
	return((fID < 0) ? NULL : fDev);
}

