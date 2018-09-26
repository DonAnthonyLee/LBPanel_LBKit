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
 * File: OLEDListView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_LIST_VIEW_H__
#define __OLED_LIST_VIEW_H__

#include <OLEDPageView.h>
#include <OLEDListItem.h>

#ifdef __cplusplus /* Just for C++ */

class OLEDListView : public OLEDPageView, public BInvoker {
public:
	OLEDListView(const char *name = NULL);
	virtual ~OLEDListView();

	bool			AddItem(OLEDListItem *item);
	bool			AddItem(OLEDListItem *item, int32 index);
	bool			RemoveItem(OLEDListItem *item);
	OLEDListItem*		RemoveItem(int32 index);

	OLEDListItem*		ItemAt(int32 index) const;
	int32			CountItems() const;
	int32			IndexOf(OLEDListItem *item) const;

	bool			SwapItems(int32 indexA, int32 indexB);
	bool			MoveItem(int32 fromIndex, int32 toIndex);
	void			SortItems(int (*cmpFunc)(const OLEDListItem**, const OLEDListItem**));

	void			ShowItem(int32 index);
	void			HideItem(int32 index);

	// Position(): When it's selectable, the current position means the current selection.
	void			SetPosition(int32 pos);
	int32			Position() const;

	bool			IsSelectable() const;
	bool			MakeSelectable(bool state);

	BMessage*		SelectionMessage() const;
	void			SetSelectionMessage(BMessage *message);

	void			SetInvocationMessage(BMessage *message);
	BMessage*		InvocationMessage() const;
	virtual status_t	Invoke(const BMessage *message = NULL);

	// Empty functions BEGIN --- just for derived class
	virtual void		SelectionChanged();
	// Empty functions END

	virtual void		Draw(BRect updateRect);
	virtual void		KeyDown(uint8 key, uint8 clicks);
	virtual void		KeyUp(uint8 key, uint8 clicks);
	virtual void		StandIn();
	virtual void		Activated(bool state);

protected:
	virtual void		RefreshNavButtonIcons();

	// TODO: Similar to OLEDMenuView ...
	int32			CountVisibleItems(int32 fromIndex, int32 n) const;
	OLEDListItem*		PrevVisibleItem(int32 &index) const;
	OLEDListItem*		NextVisibleItem(int32 &index) const;
	OLEDListItem*		FirstVisibleItem(int32 &index) const;
	OLEDListItem*		LastVisibleItem(int32 &index) const;

private:
	BList fItems;
	int32 fOffset;
	int32 fPos;
	int32 fSelectable;
	BMessage *fSelectionMessage;

	void			ResetOffsetIfNeeded();
};

#endif /* __cplusplus */

#endif /* __OLED_LIST_VIEW_H__ */

