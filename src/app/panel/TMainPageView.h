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

#define LBPANEL_MAJOR_VERSION		0
#define LBPANEL_MINOR_VERSION		1

#ifdef __cplusplus /* Just for C++ */

class TMainPageView : public LBPageView {
public:
	TMainPageView(const char *name = NULL);
	virtual ~TMainPageView();

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	FlexibleKeyDown(uint16 key, uint8 clicks);
	virtual void	FlexibleKeyUp(uint16 key, uint8 clicks);
	virtual void	Pulse();
	virtual void	Activated(bool state);

	void		Set24Hours(bool state = true);
	void		ShowSeconds(bool state = true);
	void		SetThermalZone(int32 zone);

	virtual void	MessageReceived(BMessage *msg);

private:
	int32 fTabIndex;
	bool f24Hours;
	bool fShowSeconds;
	bigtime_t fShowTimestamp;
	int32 fThermalZone;

	BString fDate;
	BString fTime;
	BString fWeek;

	int32 fInterfacesCount;
	bigtime_t fInterfaceCheckTimestamp;
	BString fInterfaceNames;

	int32 fCPUSCount;
	bigtime_t *fCPUTime;

	void UpdatePulseRate(bigtime_t rate = (bigtime_t)-1);

	void GetTime(BString*, BString*, BString*) const;

	void DrawBoardInfo(BRect r);
	void DrawDateAndTime(BRect r);
	void DrawCPUInfo(BRect r);
	void DrawInterfaceInfo(BRect r, int32 id);

	int32 GetInterfacesCount() const;
	bool GetInterfaceName(BString &ifname, int32 id) const;
	bool GetInterfaceHWAddr(BString &hwaddr, const char *ifname) const;
	bool GetInterfaceIPv4(BString &ipaddr, const char *ifname) const;
	bool GetInterfaceIPv6(BString &ipaddr, const char *ifname) const;

	bool CheckInterfaces(bool force = false);

	void ConfirmedRequested();
	void PowerOffRequested();
};

#endif /* __cplusplus */

#endif /* __T_MAIN_PAGE_VIEW_H__ */

