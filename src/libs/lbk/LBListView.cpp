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

/*
 * Default behavior of keypad control:
 *	Count of keys = 1:
 *		Single click to switch to next item if the id of icon is LBK_ICON_NONE.
 *		Double clicks to cancel if AutoStandBack set and the id of icon is LBK_ICON_NONE.
 *		Long press to launch if the id of icon is LBK_ICON_NONE.
 *	Count of keys = 2:
 *		At default, UP for the first key, DOWN for the last key.
 *		Single click at UP/DOWN to switch to previous/next item.
 *		Double clicks at the first key to launch, double clicks at the last key to cancel if AutoStandBack set.
 *		Long press at UP/DOWN to switch to first/last item.
 *	Count of keys >= 3:
 *		At default, UP for the the antepenultimate key, OK for the penultimate key, DOWN for the last key.
 *		Single click at UP/DOWN to switch to previous/next item, single click at OK to launch if selectable.
 *		Double clicks at OK to cancel if AutoStandBack set.
 *		Long press at UP/DOWN to switch to first/last item.
 */
 

LBListView::LBListView(int32 visible_items_count, const char *name)
	: LBPageView(name),
	  BInvoker(),
	  LBScopeHandler(),
	  fSelectable(false),
	  fVisibleItemsCount(visible_items_count),
	  fItemsAlignment(B_ALIGN_LEFT),
	  fBorderStyle(LBK_LIST_VIEW_NO_BORDER),
	  fSelectionMessage(NULL),
	  fAutoStandBack(true)
{
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
		RefreshNavButtonIcons();
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
			aItem->Draw(r, k);
			if(aItem == curItem)
				DrawSelection(r, k);
		}
		r.OffsetBy(0, r.Height() + 1.f);
	}
}


void
LBListView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	uint8 count = CountPanelKeys();
	if(count < 3 || key >= count) return; 

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(IsNavButtonHidden(key) || !(btnIcon == LBK_ICON_UP || btnIcon == LBK_ICON_DOWN)) return;

	if(clicks == 1)
	{
		if(btnIcon == LBK_ICON_UP)
			RollDown();
		else // btnIcon == LBK_ICON_DOWN
			RollUp();
	}
	else if(clicks == 0xff)
	{
		int32 pos = -1;
		if(btnIcon == LBK_ICON_UP)
			FirstVisibleItem(pos);
		else // btnIcon == LBK_ICON_DOWN
			LastVisibleItem(pos);
		SetPosition(pos);
	}
}

void
LBListView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	uint8 count = CountPanelKeys();
	if(count == 0 || key >= count) return; 

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(count > 2 && (IsNavButtonHidden(key) || btnIcon == LBK_ICON_NONE)) return;

	switch(count)
	{
		case 1:
			if(btnIcon != LBK_ICON_NONE) return;
			if(clicks == 0xff) break;
			if(clicks == 1)
			{
				int32 pos = Position();
				if(NextVisibleItem(pos) == NULL)
					FirstVisibleItem(pos);
				SetPosition(pos);
			}
			else if(clicks > 1 && clicks != 0xff && fAutoStandBack)
			{
				StandBack();
			}
			return;

		default:
			if(count == 2)
			{
				if(clicks > 1 && clicks != 0xff && key == 0) break;
				if(btnIcon == LBK_ICON_UP || btnIcon == LBK_ICON_DOWN)
				{
					if(clicks == 1)
					{
						if(btnIcon == LBK_ICON_UP)
							RollDown();
						else // btnIcon == LBK_ICON_DOWN
							RollUp();
					}
					else if(clicks == 0xff)
					{
						int32 pos = -1;
						if(btnIcon == LBK_ICON_UP)
							FirstVisibleItem(pos);
						else // btnIcon == LBK_ICON_DOWN
							LastVisibleItem(pos);
						SetPosition(pos);
					}
				}
			}
			if(clicks == 1 && btnIcon == LBK_ICON_OK) break;
			if(fAutoStandBack && clicks > 1 && clicks != 0xff &&
			   ((count == 2 && key == 1) ||
			    (count > 2 && btnIcon == LBK_ICON_OK)))
				StandBack();
			return;
	}

	if(fSelectable)
	{
		LBListItem *curItem = CurrentSelection();
		if(curItem != NULL) Invoke((const BMessage*)NULL);
	}
}


void
LBListView::FlexibleKeyDown(uint16 key, uint8 clicks)
{
	int32 pos = -1;

	switch(key)
	{
		case B_UP_ARROW:
			RollDown();
			break;

		case B_DOWN_ARROW:
			RollUp();
			break;

		case B_HOME:
		case B_LEFT_ARROW:
			FirstVisibleItem(pos);
			SetPosition(pos);
			break;

		case B_END:
		case B_RIGHT_ARROW:
			LastVisibleItem(pos);
			SetPosition(pos);
			break;

		default:
			break;
	}
}


void
LBListView::FlexibleKeyUp(uint16 key, uint8 clicks)
{
	LBListItem *curItem;

	switch(key)
	{
		case B_ENTER:
			if(fSelectable == false || (curItem = CurrentSelection()) == NULL) break;
			Invoke((const BMessage*)NULL);
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
	uint8 count = CountPanelKeys();
	if(count < 2) return;

	int32 n = (Position() <= 0 ? 0 : CountVisibleItems(0, Position()));
	for(uint8 k = 0; k < count; k++)
	{
		switch(GetNavButtonIcon((int32)k))
		{
			case LBK_ICON_UP:
				if(n > 0)
					ShowNavButton(k);
				else
					HideNavButton(k);
				break;

			case LBK_ICON_OK:
				if(CurrentSelection() != NULL && fSelectable)
					ShowNavButton(k);
				else
					HideNavButton(k);
				break;

			case LBK_ICON_DOWN:
				if(CountVisibleItems(Position() + 1, -1) > 0)
					ShowNavButton(k);
				else
					HideNavButton(k);
				break;

			default:
				break;
		}
	}
}


void
LBListView::SelectionChanged()
{
	// EMPTY
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
			Invoke((const BMessage*)fSelectionMessage);
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
	const BMessage *message = (msg ? msg : Message());
	if(message == NULL) return B_BAD_VALUE;

	BMessage aMsg(*message);
	aMsg.AddInt32("index", Position());

	status_t st = BInvoker::Invoke(&aMsg);
	if(st == B_OK && fAutoStandBack) StandBack();
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
	DrawIcon(LBK_ICON_SMALL_RIGHT, r.LeftTop() + BPoint(r.Width() / 2.f, r.Height() / 2.f) - BPoint(4, 4));

	r = rect;
	r.left = r.right - r.Height();
	if(r.IsValid()) DrawIcon(LBK_ICON_SMALL_LEFT, r.LeftTop() + BPoint(r.Width() / 2.f, r.Height() / 2.f) - BPoint(4, 4));

	if(fBorderStyle == LBK_LIST_VIEW_NO_BORDER) InvertRect(rect);
}


void
LBListView::SetAutoStandBack(bool state)
{
	fAutoStandBack = state;
}


void
LBListView::Attached()
{
	LBPageView::Attached();

	uint8 count = CountPanelKeys();
	switch(count)
	{
		case 2:
			// TODO: orientation, etc.
			SetNavButtonIcon(0, LBK_ICON_UP);
			SetNavButtonIcon(1, LBK_ICON_DOWN);
			break;

		default:
			// TODO: orientation, etc.
			if(count < 3 || count > LBK_KEY_MAXIMUM_NUMBER) break;
			SetNavButtonIcon(count - 3, LBK_ICON_UP);
			SetNavButtonIcon(count - 2, LBK_ICON_OK);
			if(fSelectable == false)
				HideNavButton(count - 2);
			SetNavButtonIcon(count - 1, LBK_ICON_DOWN);
	}
}
