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
 * File: OLEDApp.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_APP_H__
#define __OLED_APP_H__

#include <be/Be.h>
#include "OLEDView.h"

#ifdef __cplusplus /* Just for C++ */

class OLEDApp : public BLooper {
public:
	OLEDApp(int oled_fd, int input_fd);
	virtual ~OLEDApp();

	bool		AddPageView(OLEDView *view, bool left_side = true);
	bool		RemovePageView(OLEDView *view);
	OLEDView*	RemovePageView(int32 index, bool left_side = true);
	OLEDView*	PageViewAt(int32 index, bool left_side = true) const;
	int32		CountPageViews(bool left_side = true) const;

	void		ActivatePageView(int32 index, bool left_side = true);
	OLEDView*	GetActivatedPageView() const;

	void		Go();

	// TODO

	virtual void	MessageReceived(BMessage *msg);

private:
	int fOLEDFD;
	int fInputFD;
	int fPipes[2];

	uint8 fKeyState;
	bigtime_t fKeyTimestamps[8];
	uint8 fKeyClicks[8];

	BList fLeftPageViews;
	BList fRightPageViews;
};

#endif /* __cplusplus */

#endif /* __OLED_APP_H__ */

