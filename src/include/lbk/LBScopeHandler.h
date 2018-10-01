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
 * File: LBScopeHandler.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_SCOPE_HANDLER_H__
#define __LBK_SCOPE_HANDLER_H__

#include <lbk/LBScopeItem.h>

#ifdef __cplusplus /* Just for C++ */

class _EXPORT LBScopeHandler : public LBScopeItem {
public:
	LBScopeHandler(const char *name = NULL);
	virtual ~LBScopeHandler();

	bool			AddItem(LBScopeItem *item);
	bool			RemoveItem(LBScopeItem *item);

	virtual bool		AddItem(LBScopeItem *item, int32 index);
	virtual LBScopeItem*	RemoveItem(int32 index);

	LBScopeItem*		ItemAt(int32 index) const;
	int32			CountItems() const;
	int32			IndexOf(LBScopeItem *item) const;

	bool			SwapItems(int32 indexA, int32 indexB);
	bool			MoveItem(int32 fromIndex, int32 toIndex);
	void			SortItems(int (*cmpFunc)(const LBScopeItem**, const LBScopeItem**));
	void			DoForEach(bool (*func)(LBScopeItem *item));

	void			SetPosition(int32 pos);
	int32			Position() const;

	void			SetOffset(int32 offset);
	int32			Offset() const;

	virtual void		RollUp();
	virtual void		RollDown();

	// Empty functions BEGIN --- just for derived class
	virtual void		PositionChanged(int32 pos, int32 old);
	virtual void		OffsetChanged(int32 offset, int32 old);
	virtual void		ScopeChanged();
	// Empty functions END

	void			ShowItem(LBScopeItem *item);
	void			HideItem(LBScopeItem *item);
	virtual void		ShowItem(int32 index);
	virtual void		HideItem(int32 index);
	bool			IsItemVisible(const LBScopeItem *item) const;

	virtual void		Expand();
	virtual void		Collapse();

	virtual void		Show();
	virtual void		Hide();

protected:
	virtual int32		VisibleItemsCountMax() const;

	int32			CountVisibleItems(int32 fromIndex, int32 n) const;
	LBScopeItem*		PrevVisibleItem(int32 &index) const;
	LBScopeItem*		NextVisibleItem(int32 &index) const;
	LBScopeItem*		FirstVisibleItem(int32 &index, int32 fromOffset = 0) const;
	LBScopeItem*		LastVisibleItem(int32 &index) const;
	LBScopeItem*		FirstVisibleItemAtScope(int32 &index) const;
	LBScopeItem*		LastVisibleItemAtScope(int32 &index) const;

private:
	BList fItems;
	int32 fOffset;
	int32 fPos;

	virtual int32		RejustOffsetAfterPositionChanged();
};

#endif /* __cplusplus */

#endif /* __LBK_SCOPE_HANDLER_H__ */

