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
 * File: LBView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_VIEW_H__
#define __LBK_VIEW_H__

#include <lbk/LBKConfig.h>
#include <lbk/LBIconDefs.h>
#include <lbk/add-ons/LBPanelDevice.h>

#ifdef __cplusplus /* Just for C++ */

class LBApplication;

class _EXPORT LBView : public BHandler {
public:
	LBView(const char *name = NULL);
	virtual ~LBView();

	// Bounds(): A derived class usually draw within it.
	virtual BRect	Bounds() const;

#ifdef LBK_ENABLE_MORE_FEATURES
	void		SetHighColor(rgb_color c);
	void		SetHighColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	void		SetLowColor(rgb_color c);
	void		SetLowColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	void		SetViewColor(rgb_color c);
	void		SetViewColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	rgb_color	HighColor() const;
	rgb_color	LowColor() const;
	rgb_color	ViewColor() const;
#endif

	void		FillRect(BRect r, pattern p = B_SOLID_HIGH);
	void		StrokeRect(BRect r, bool erase = false);
	void		DrawString(const char *string, BPoint location, bool erase = false);
	void		DrawIcon(const lbk_icon *icon, BPoint location);

	virtual void	DrawIcon(lbk_icon_id idIcon, BPoint location);

	uint8		FontSize() const;
	void		SetFontSize(uint8 size);
	uint16		StringWidth(const char *string) const;

	void		EnableUpdate(bool state);
	bool		IsNeededToRegen() const;

	bool		GetPowerState() const;
	void		SetPowerState(bool state);

	// Empty functions BEGIN --- just for derived class
	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	Pulse();
	// Empty functions END

	uint8		KeyState(uint8 *down_state = NULL) const;

	bool		IsActivated() const;
	virtual void	Activated(bool state);

	void		Invalidate();
	void		Invalidate(BRect r);

	bool		AddStickView(LBView *view);
	bool		RemoveStickView(LBView *view);
	LBView*		RemoveStickView(int32 index);
	int32		CountStickViews() const;
	LBView*		StickViewAt(int32 index) const;
	LBView*		FindStickView(const char *name) const;

	bool		IsStoodIn() const;
	LBView*		StandingInView() const;
	virtual void	StandIn();
	virtual void	StandBack();

	LBView*		MasterView() const;
	LBView*		TopView() const;

	virtual void	Attached();
	virtual void	Detached();

	virtual void	MessageReceived(BMessage *msg);

protected:
	bigtime_t	GetStandInTime() const;
	LBPanelDevice*	PanelDevice() const;
	uint8		CountPanelKeys() const;

private:
	friend class LBApplication;

	LBPanelDevice *fDev;
	bigtime_t fTimestamp;
	bool fActivated;

	uint8 fFontSize;
	uint16 fKeyState;
	uint32 fUpdateCount;
	BRect fUpdateRect;

	LBView *fMasterView;
	LBView *fStandingInView;
	BList fStickViews;
	bigtime_t fStandInTimestamp;

#ifdef LBK_ENABLE_MORE_FEATURES
	rgb_color fColors[3];
#endif

	void		SetActivated(bool state);
	void		Clear(BRect r);
};

#endif /* __cplusplus */

#endif /* __LBK_VIEW_H__ */

