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
 * File: OLEDMenuView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_MENU_VIEW_H__
#define __OLED_MENU_VIEW_H__

#include <OLEDPageView.h>
#include <OLEDMenuItem.h>

#ifdef __cplusplus /* Just for C++ */

class OLEDMenuView : public OLEDPageView {
public:
	OLEDMenuView(const char *name = NULL);
	virtual ~OLEDMenuView();

	bool		AddItem(OLEDMenuItem *item);
	bool		AddItem(OLEDMenuItem *item, int32 index);
	bool		RemoveItem(OLEDMenuItem *item);
	OLEDMenuItem*	RemoveItem(int32 index);

	OLEDMenuItem*	ItemAt(int32 index) const;
	int32		CountItems() const;
	int32		IndexOf(OLEDMenuItem *item) const;

	OLEDMenuItem*	FindItem(uint32 command) const;
	OLEDMenuItem*	CurrentSelection() const;

	void		ShowItem(int32 index);
	void		HideItem(int32 index);

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	StandIn();
	virtual void	Activated(bool state);

protected:
	virtual void	ItemInvoked(OLEDMenuItem *item);

private:
	BList fItems;
	int32 fSelected;
	int32 fOffset;

	int32		CountVisibleItems(int32 fromIndex, int32 n) const;
	OLEDMenuItem*	PrevVisibleItem(int32 &index) const;
	OLEDMenuItem*	NextVisibleItem(int32 &index) const;
	OLEDMenuItem*	FirstVisibleItem(int32 &index) const;
	OLEDMenuItem*	LastVisibleItem(int32 &index) const;
	void		RefreshNavButtons();
	void		ResetOffsetIfNeeded();
};

#endif /* __cplusplus */

#endif /* __OLED_MENU_VIEW_H__ */

