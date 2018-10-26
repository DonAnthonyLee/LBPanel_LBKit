/* --------------------------------------------------------------------------
 *
 * Panel application for little board
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
 * File: TMainPageView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __T_MAIN_PAGE_VIEW_H__
#define __T_MAIN_PAGE_VIEW_H__

#include <lbk/LBKit.h>

#ifdef __cplusplus /* Just for C++ */

class TMainPageView : public LBPageView {
public:
	TMainPageView(const char *name = NULL);
	virtual ~TMainPageView();

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	Pulse();
	virtual void	Activated(bool state);

	void		Set24Hours(bool state = true);
	void		ShowSeconds(bool state = true);

	// TODO
	virtual void	MessageReceived(BMessage *msg);

private:
	int32 fTabIndex;
	bool f24Hours;
	bool fShowSeconds;
	bigtime_t fShowTimestamp;

	BString fDate;
	BString fTime;
	BString fWeek;
	int32 fInterfacesCount;
	int32 fCPUSCount;
	bigtime_t *fCPUTime;

	void GetTime(BString*, BString*, BString*) const;

	void DrawBoardInfo(BRect r);
	void DrawDateAndTime(BRect r);
	void DrawCPUInfo(BRect r);
	void DrawInterfaceInfo(BRect r, int32 id);
};

#endif /* __cplusplus */

#endif /* __T_MAIN_PAGE_VIEW_H__ */

