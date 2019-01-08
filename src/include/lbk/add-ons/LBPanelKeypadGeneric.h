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
 * File: LBPanelKeypadGeneric.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_KEYPAD_GENERIC_H__
#define __LBK_PANEL_DEVICE_KEYPAD_GENERIC_H__

#include <lbk/add-ons/LBPanelKeypad.h>

#ifdef __cplusplus /* Just for C++ */

class _EXPORT LBPanelKeypadGeneric : public LBPanelKeypad {
public:
	LBPanelKeypadGeneric(const char *dev);
	virtual ~LBPanelKeypadGeneric();

	virtual status_t	InitCheck(const char *options);

	void			SetKeycode(int32 id, uint16 keycode);

	virtual status_t	GetCountOfKeys(uint8 &count);

private:
	int fFD;
	int fPipes[2];
	void *fThread;
	uint16 fKeycodes[LBK_KEY_MAXIMUM_NUMBER];

	static int32		InputEventsObserver(void*);
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_KEYPAD_GENERIC_H__ */

