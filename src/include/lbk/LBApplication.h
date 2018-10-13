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
 * File: LBApplication.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_APPLICATION_H__
#define __LBK_APPLICATION_H__

#include <lbk/LBView.h>

#ifdef __cplusplus /* Just for C++ */

#define LBK_QUIT_REQUESTED		'lBKq'
#define LBK_APP_SETTINGS_UPDATED	'lBKs'

class LBPanelDevice;

class _EXPORT LBApplication : public BLooper {
public:
	LBApplication(const BList *cfg);
	virtual ~LBApplication();

	bool		AddPageView(LBView *view, bool left_side = true, int32 panel_index = 0);
	bool		RemovePageView(LBView *view);
	LBView*		RemovePageView(int32 index, bool left_side = true, int32 panel_index = 0);
	LBView*		PageViewAt(int32 index, bool left_side = true, int32 panel_index = 0) const;
	int32		CountPageViews(bool left_side = true, int32 panel_index = 0) const;

	void		ActivatePageView(int32 index, bool left_side = true, int32 panel_index = 0);
	LBView*		GetActivatedPageView(int32 panel_index = 0) const;

	void		Go();

	bigtime_t	PulseRate() const;
	void		SetPulseRate(bigtime_t rate);

	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);

protected:
	int32		CountPanels() const;
	LBPanelDevice*	PanelAt(int32 index) const;
	uint8		CountPanelKeys(int32 index) const;

private:
	bool fQuitLooper;
	BList fAddOnsList;
	int fPipes[2];
	bigtime_t fPulseRate;
	int32 fPanelsCount;
	void *fIPC;
};

#endif /* __cplusplus */

#endif /* __LBK_APPLICATION_H__ */

