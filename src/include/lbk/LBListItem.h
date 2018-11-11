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
 * File: LBListItem.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_LIST_ITEM_H__
#define __LBK_LIST_ITEM_H__

#include <lbk/LBIconDefs.h>
#include <lbk/LBScopeItem.h>

#if defined(ETK_MAJOR_VERSION) && defined(ETK_LITE_BUILD)
enum {
	B_CONTROL_OFF = 0,
	B_CONTROL_ON = 1
};
#endif

#ifdef __cplusplus /* Just for C++ */

class LBListView;


class _EXPORT LBListItem : public LBScopeItem {
public:
	LBListItem();
	virtual ~LBListItem();

	bool			IsHidden() const;

protected:
	void			DrawString(const char *str, BRect r, int32 n);
	void			Invalidate();

private:
	friend class LBListView;

	virtual void		Draw(BRect r, int32 n) = 0;
};


class _EXPORT LBListStringItem : public LBListItem {
public:
	LBListStringItem(const char *text);
	virtual ~LBListStringItem();

	const char*		Text() const;
	virtual void		SetText(const char *text);

	void			SetIcon(lbk_icon_id icon);
	void			RemoveIcon();

protected:
	virtual void		DrawIcon(LBListView *owner,
					 lbk_icon_id icon, BPoint location);

private:
	char *fText;
	bool fHasIcon;
	lbk_icon_id fIcon;

	virtual void		Draw(BRect r, int32 n);
};


class _EXPORT LBListControlItem : public LBListItem {
public:
	LBListControlItem(const char *label, int32 value = B_CONTROL_OFF);
	virtual ~LBListControlItem();

	const char*		Label() const;
	virtual void		SetLabel(const char *label);

	int32			Value() const;
	virtual void		SetValue(int32 value);

private:
	char *fLabel;
	int32 fValue;

	virtual void		Draw(BRect r, int32 n);
};


#endif /* __cplusplus */

#endif /* __LBK_LIST_ITEM_H__ */

