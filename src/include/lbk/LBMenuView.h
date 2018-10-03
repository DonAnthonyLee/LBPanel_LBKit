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
 * File: LBMenuView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_MENU_VIEW_H__
#define __LBK_MENU_VIEW_H__

#include <lbk/LBPageView.h>
#include <lbk/LBMenuItem.h>
#include <lbk/LBScopeHandler.h>

#ifdef __cplusplus /* Just for C++ */

class _EXPORT LBMenuView : public LBPageView, public LBScopeHandler {
public:
	LBMenuView(const char *name = NULL);
	virtual ~LBMenuView();

	LBMenuItem*	FindItem(uint32 command) const;
	LBMenuItem*	CurrentSelection() const;

	virtual BRect	ItemLabelBounds() const;
	virtual BRect	ItemIconBounds() const;

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	StandIn();
	virtual void	Activated(bool state);
	virtual void	Attached();

protected:
	virtual void	ItemInvoked(LBMenuItem *item);
	virtual void	RefreshNavButtonIcons();

	virtual int32	VisibleItemsCountMax() const;
	virtual void	PositionChanged(int32 pos, int32 old);
	virtual void	OffsetChanged(int32 offset, int32 old);
	virtual void	ScopeChanged();
	virtual bool	IsValidKind(LBScopeItem *item) const;
};

#endif /* __cplusplus */

#endif /* __LBK_MENU_VIEW_H__ */

