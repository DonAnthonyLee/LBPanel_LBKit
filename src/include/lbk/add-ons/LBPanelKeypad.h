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
 * File: LBPanelKeypad.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_KEYPAD_H__
#define __LBK_PANEL_DEVICE_KEYPAD_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus /* Just for C++ */

// NOTE:
// 	Addon must have C function like below and all the null virtual functions
// 	of class must be implemented.
// 		extern "C" _EXPORT LBPanelKeypad* instantiate_panel_keypad();

class LBPanelCombiner;

class _EXPORT LBPanelKeypad {
public:
	LBPanelKeypad();
	virtual ~LBPanelKeypad();

	virtual status_t	InitCheck(const char *options) = 0;

	virtual status_t	GetCountOfKeys(uint8 &count) = 0;
	virtual status_t	BlockKeyEvents(bool state) = 0;

	status_t		SendMessageToApp(const BMessage *msg);
	status_t		SendMessageToApp(uint32 command);

private:
	friend class LBPanelCombiner;

	BMessenger fMsgr;
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_KEYPAD_H__ */

