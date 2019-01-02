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
 * File: LBPanelKeypad.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelKeypad.h>


LBPanelKeypad::LBPanelKeypad()
	: fID(-1), fDev(NULL)
{
	// TODO
}


LBPanelKeypad::~LBPanelKeypad()
{
	// TODO
}


status_t
LBPanelKeypad::SendMessageToApp(const BMessage *msg)
{
	if(msg == NULL || fID < 0 || fDev == NULL) return B_BAD_VALUE;

	BMessage aMsg(*msg);
#ifdef ETK_MAJOR_VERSION
	aMsg.RemoveData("keypad_id");
#else
	aMsg.RemoveName("keypad_id");
#endif
	aMsg.AddInt32("keypad_id", fID);

	return fDev->SendMessageToApp(&aMsg);
}


status_t
LBPanelKeypad::SendMessageToApp(uint32 command)
{
	BMessage msg(command);
	return SendMessageToApp(&msg);
}


LBPanelDevice*
LBPanelKeypad::Panel() const
{
	return((fID < 0) ? NULL : fDev);
}

