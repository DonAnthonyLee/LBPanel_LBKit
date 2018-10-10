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
 * File: LBListView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKConfig.h>
#include <lbk/LBListView.h>

// Comment the line below out when it's SURE to use it
#if LBK_KEY_TYPICAL_NUMBER < 2
#error "LBListView: Usually, it's useless when number of keys less than 2 !!!"
#endif


LBListView::LBListView(int32 visible_items_count, const char *name)
	: LBPageView(name),
	  BInvoker(),
	  LBScopeHandler(),
	  fSelectable(false),
	  fVisibleItemsCount(visible_items_count),
	  fItemsAlignment(B_ALIGN_LEFT),
	  fBorderStyle(LBK_LIST_VIEW_NO_BORDER),
	  fSelectionMessage(NULL)
{
#if LBK_KEY_TYPICAL_NUMBER == 2
	SetNavButtonIcon(0, LBK_ICON_UP);
	SetNavButtonIcon(1, LBK_ICON_DOWN);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
	SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 3, LBK_ICON_UP);
	SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 1, LBK_ICON_DOWN);
#endif
}


LBListView::~LBListView()
{
	if(fSelectionMessage != NULL) delete fSelectionMessage;
}


bool
LBListView::IsValidKind(LBScopeItem *item) const
{
	return(is_kind_of(item, LBListItem));
}


LBListItem*
LBListView::CurrentSelection() const
{
	if(fSelectable == false) return NULL;

	LBScopeItem *item = ItemAt(Position());
	if(item == NULL || item->IsVisible() == false) return NULL;
	return cast_as(item, LBListItem);
}


bool
LBListView::IsSelectable() const
{
	return fSelectable;
}


void
LBListView::MakeSelectable(bool state)
{
	if(fSelectable != state)
	{
		fSelectable = state;
#if LBK_KEY_TYPICAL_NUMBER >= 3
		if(fSelectable)
			SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 2,
					 fSelectable ? LBK_ICON_OK : LBK_ICON_NONE);
#endif
		Invalidate(Bounds());
	}
}


void
LBListView::Draw(BRect rect)
{
	LBPageView::Draw(rect);
	if(CountItems() == 0 || fVisibleItemsCount == 0) return;

	BRect r;
	LBListItem *curItem = CurrentSelection();

	r = Bounds();
	r.bottom = r.Height() / (float)fVisibleItemsCount - 1.f;

	int32 index, k = 0;
	for(LBScopeItem *item = FirstVisibleItemAtScope(index);
	    item != NULL;
	    item = NextVisibleItemAtScope(index), k++)
	{
		if(r.Intersects(rect))
		{
			DrawItemBorder(r, k);

			LBListItem *aItem = cast_as(item, LBListItem);
			if(aItem == curItem)
				DrawSelection(r, k);
			aItem->Draw(r, k);
		}
		r.OffsetBy(0, r.Height() + 1.f);
	}
}


void
LBListView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	if(clicks != 0xff) return;
	if(IsNavButtonHidden(key)) return;

	int32 pos = -1;
	switch(GetNavButtonIcon((int32)key))
	{
		case LBK_ICON_UP:
			FirstVisibleItem(pos);
			break;

		case LBK_ICON_DOWN:
			LastVisibleItem(pos);
			break;

		default:
			return;
	}
	SetPosition(pos);
}

void
LBListView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	if(IsNavButtonHidden(key)) return;

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(btnIcon == LBK_ICON_NONE) return;

	if(fSelectable)
	{
#if LBK_KEY_TYPICAL_NUMBER >= 3
		if(clicks == 1 && btnIcon == LBK_ICON_OK)
#else // LBK_KEY_TYPICAL_NUMBER < 3
		if((clicks == 1 && btnIcon == LBK_ICON_OK) ||
		   ((clicks > 1 && clicks != 0xff) && btnIcon == LBK_ICON_UP))
#endif
		{
			LBListItem *curItem = CurrentSelection();
			if(curItem != NULL) Invoke();
			return;
		}
	}
	if(clicks > 1) return;

	switch(btnIcon)
	{
		case LBK_ICON_UP:
			RollDown();
			break;

		case LBK_ICON_DOWN:
			RollUp();
			break;

		default:
			break;
	}
}


void
LBListView::StandIn()
{
	LBPageView::StandIn();

	if(Position() < 0)
	{
		int32 pos = -1;

		FirstVisibleItem(pos);
		SetPosition(pos);
		RefreshNavButtonIcons();
	}
}


void
LBListView::Activated(bool state)
{
	LBPageView::Activated(state);

	if(state && Position() < 0)
	{
		int32 pos = -1;

		FirstVisibleItem(pos);
		SetPosition(pos);
		RefreshNavButtonIcons();
	}
}


void
LBListView::RefreshNavButtonIcons()
{
	int32 n = (Position() <= 0 ? 0 : CountVisibleItems(0, Position()));

	// UP
	if(n > 0)
	{
#if LBK_KEY_TYPICAL_NUMBER == 2
		ShowNavButton(0);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
		ShowNavButton(LBK_KEY_TYPICAL_NUMBER - 3);
#endif
	}
	else
	{
#if LBK_KEY_TYPICAL_NUMBER == 2
		HideNavButton(0);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
		HideNavButton(LBK_KEY_TYPICAL_NUMBER - 3);
#endif
	}

	// OK
#if LBK_KEY_TYPICAL_NUMBER >= 3
	if(CurrentSelection() != NULL)
		ShowNavButton(LBK_KEY_TYPICAL_NUMBER - 2);
	else
		HideNavButton(LBK_KEY_TYPICAL_NUMBER - 2);
#endif

	// DOWN
	n = CountVisibleItems(Position() + 1, -1);
	if(n > 0)
	{
#if LBK_KEY_TYPICAL_NUMBER == 2
		ShowNavButton(1);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
		ShowNavButton(LBK_KEY_TYPICAL_NUMBER - 1);
#endif
	}
	else
	{
#if LBK_KEY_TYPICAL_NUMBER == 2
		HideNavButton(1);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
		HideNavButton(LBK_KEY_TYPICAL_NUMBER - 1);
#endif
	}
}


void
LBListView::SelectionChanged()
{
}


int32
LBListView::VisibleItemsCountMax() const
{
	return fVisibleItemsCount;
}


void
LBListView::PositionChanged(int32 pos, int32 old)
{
	Invalidate(Bounds());
	RefreshNavButtonIcons();
	if(fSelectable)
	{
		if(fSelectionMessage != NULL)
			Invoke(fSelectionMessage);
		SelectionChanged();
	}
}


void
LBListView::OffsetChanged(int32 offset, int32 old)
{
	Invalidate(Bounds());
}


void
LBListView::ScopeChanged()
{
	RefreshNavButtonIcons();
}


BMessage*
LBListView::SelectionMessage() const
{
	return fSelectionMessage;
}


void
LBListView::SetSelectionMessage(BMessage *message)
{
	if(fSelectionMessage != NULL) delete fSelectionMessage;
	fSelectionMessage = message;
}


status_t
LBListView::Invoke(const BMessage *msg)
{
	const EMessage *message = (msg ? msg : Message());
	if(message == NULL) return B_BAD_VALUE;

	BMessage aMsg(*message);
	aMsg.AddInt32("index", Position());

	status_t st = EInvoker::Invoke(&aMsg);
	if(st == B_OK) StandBack();
	return st;
}


void
LBListView::InvalidateItem(int32 index)
{
#if 0
	if(index < Offset() || fVisibleItemsCount == 0) return;
#else
	if(IsItemVisible(ItemAt(index)) == false) return;
#endif

	int32 count = CountVisibleItems(Offset(), index - Offset() + 1);
#if 0
	if(count > fVisibleItemsCount) return;
#endif

	BRect r = Bounds();
	r.bottom = r.Height() / (float)fVisibleItemsCount - 1.f;

	while(count-- > 1) r.OffsetBy(0, r.Height() + 1.f);
	Invalidate(r);
}


alignment
LBListView::ItemsAlignment() const
{
	return fItemsAlignment;
}


void
LBListView::SetItemsAlignment(alignment align)
{
	if(fItemsAlignment == align) return;
	fItemsAlignment = align;
	Invalidate(Bounds());
}


uint8
LBListView::BorderStyle() const
{
	return fBorderStyle;
}


void
LBListView::SetBorderStyle(uint8 style)
{
	if(fBorderStyle == style || style >= LBK_LIST_VIEW_BORDER_STYLE_MAX_ID) return;
	fBorderStyle = style;
	Invalidate(Bounds());
}


void
LBListView::DrawItemBorder(BRect r, int32 n)
{
	if(fSelectable)
	{
		r.left += r.Height() + 1.f;
		r.right -= r.Height() + 1.f;
		if(r.IsValid() == false) return;
	}

	switch(fBorderStyle)
	{
		case LBK_LIST_VIEW_FORM:
			if(n > 0) r.top -= 1.f;
			StrokeRect(r, false);
			break;

		case LBK_LIST_VIEW_INTERLACED_ROWS:
			if((n & 0x01) == 0) break;
			FillRect(r, B_SOLID_HIGH);
			break;

		default:
			break;
	}
}


void
LBListView::DrawSelection(BRect rect, int32 n)
{
	BRect r = rect;
	r.right = r.left + r.Height();
	DrawIcon(LBK_ICON_SMALL_RIGHT, r.Center() - BPoint(4, 4));

	r = rect;
	r.left = r.right - r.Height();
	if(r.IsValid()) DrawIcon(LBK_ICON_SMALL_LEFT, r.Center() - BPoint(4, 4));
}

