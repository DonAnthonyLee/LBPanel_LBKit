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
 * File: LBMenuView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKConfig.h>
#include <lbk/LBMenuView.h>

/*
 * Default behavior of keypad control:
 *	Count of keys = 1:
 *		Single click to switch to next item.
 *		Double clicks to cancel if AutoStandBack set.
 *		Long press to launch.
 *	Count of keys = 2:
 *		At default, LEFT for the first key, RIGHT for the last key.
 *		Single click at LEFT/RIGHT to switch to previous/next item.
 *		Double clicks at LEFT to launch, double clicks at RIGHT to cancel if AutoStandBack set.
 *		Long press at LEFT/RIGHT to switch to first/last item.
 *	Count of keys >= 3:
 *		At default, LEFT for the the antepenultimate key, OK for the penultimate key, RIGHT for the last key.
 *		Single click at LEFT/RIGHT to switch to previous/next item, single click at OK to launch.
 *		Double clicks at OK to cancel if AutoStandBack set.
 *		Long press at LEFT/RIGHT to switch to first/last item.
 */


LBMenuView::LBMenuView(const char *name)
	: LBPageView(name),
	  LBScopeHandler(),
	  fAutoStandBack(true)
{
}


LBMenuView::~LBMenuView()
{
}


bool
LBMenuView::IsValidKind(LBScopeItem *item) const
{
	return(is_kind_of(item, LBMenuItem));
}


LBMenuItem*
LBMenuView::FindItem(uint32 command) const
{
	LBMenuItem *item;

	for(int32 k = 0; k < CountItems(); k++)
	{
		item = cast_as(ItemAt(k), LBMenuItem);
		if(item->Command() == command) return item;
	}

	return NULL;
}


LBMenuItem*
LBMenuView::CurrentSelection() const
{
	LBScopeItem *item = ItemAt(Position());
	if(item == NULL || item->IsVisible() == false) return NULL;
	return cast_as(item, LBMenuItem);
}


void
LBMenuView::DrawMenuIcon(lbk_icon_id icon, BPoint location)
{
	DrawIcon(icon, location);
}


void
LBMenuView::Draw(BRect rect)
{
	LBPageView::Draw(rect);
	if(CountItems() == 0) return;

	uint16 w;
	BRect r;
	LBMenuItem *curItem = CurrentSelection();

	// item label
	r = ItemLabelBounds();
	if(!(curItem == NULL || curItem->Label() == NULL) && r.Intersects(rect))
	{
		SetFontSize(12);
		w = StringWidth(curItem->Label());
		DrawString(curItem->Label(), BPoint(r.left + r.Width() / 2.f - w / 2.f, 1));
	}

	// item icons
	r = ItemIconBounds();
	if(r.Intersects(rect) == false) return;

	int32 nMax = VisibleItemsCountMax();
	r.right = r.left + r.Width() / (float)nMax - 1.f;

	int32 index;
	for(LBScopeItem *item = FirstVisibleItemAtScope(index);
	    item != NULL;
	    item = NextVisibleItemAtScope(index))
	{
		LBMenuItem *aItem = cast_as(item, LBMenuItem);

		if(r.Intersects(rect))
		{
			DrawMenuIcon(aItem->Icon(),
				     r.LeftTop() + BPoint(r.Width() / 2.f, r.Height() / 2.f) - BPoint(15, 15));

			if(aItem == curItem)
				InvertRect(r & rect);
		}

		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
LBMenuView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	uint8 count = CountPanelKeys();
	if(count == 0 || key >= count) return; 

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(count > 1 && (IsNavButtonHidden(key) || btnIcon == LBK_ICON_NONE)) return;

	int32 pos = -1;
	switch(count)
	{
		case 1:
			if(btnIcon == LBK_ICON_NONE && clicks == 1)
			{
				pos = Position();
				if(NextVisibleItem(pos) == NULL)
					FirstVisibleItem(pos);
				break;
			}
			return;

		default:
			if(!(btnIcon == LBK_ICON_LEFT || btnIcon == LBK_ICON_RIGHT)) return;
			if(clicks == 1)
			{
				if(btnIcon == LBK_ICON_LEFT)
					RollDown();
				else // btnIcon == LBK_ICON_RIGHT
					RollUp();
			}
			if(clicks != 0xff) return;
			if(btnIcon == LBK_ICON_LEFT)
				FirstVisibleItem(pos);
			else // btnIcon == LBK_ICON_RIGHT
				LastVisibleItem(pos);
	}
	SetPosition(pos);
}

void
LBMenuView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	uint8 count = CountPanelKeys();
	if(count == 0 || key >= count) return; 

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(count > 1 && (IsNavButtonHidden(key) || btnIcon == LBK_ICON_NONE)) return;

	switch(count)
	{
		case 1:
			if(btnIcon != LBK_ICON_NONE) return;
			if(clicks == 0xff) break;
			if(clicks > 1 && clicks != 0xff && fAutoStandBack)
				StandBack();
			return;

		default:
			if(count == 2 && clicks > 1 && clicks != 0xff && btnIcon == LBK_ICON_LEFT) break;
			if(clicks == 1 && btnIcon == LBK_ICON_OK) break;
			if(fAutoStandBack && clicks > 1 && clicks != 0xff &&
			   ((count == 2 && btnIcon == LBK_ICON_RIGHT) ||
			    (count > 2 && btnIcon == LBK_ICON_OK)))
				StandBack();
			return;
	}

	LBMenuItem *curItem = CurrentSelection();
	if(curItem != NULL)
	{
		if(curItem->Invoke((const BMessage*)NULL) == B_OK)
			ItemInvoked(curItem);
	}
}


void
LBMenuView::FlexibleKeyDown(uint16 key, uint8 clicks)
{
	int32 pos = -1;

	switch(key)
	{
		case B_LEFT_ARROW:
			RollDown();
			break;

		case B_RIGHT_ARROW:
			RollUp();
			break;

		case B_HOME:
		case B_UP_ARROW:
			FirstVisibleItem(pos);
			SetPosition(pos);
			break;

		case B_END:
		case B_DOWN_ARROW:
			LastVisibleItem(pos);
			SetPosition(pos);
			break;

		default:
			break;
	}
}


void
LBMenuView::FlexibleKeyUp(uint16 key, uint8 clicks)
{
	LBMenuItem *curItem;

	switch(key)
	{
		case B_ENTER:
			if((curItem = CurrentSelection()) != NULL)
			{
				if(curItem->Invoke((const BMessage*)NULL) == B_OK)
					ItemInvoked(curItem);
			}
			break;

		default:
			break;
	}
}


void
LBMenuView::StandIn()
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
LBMenuView::Activated(bool state)
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
LBMenuView::RefreshNavButtonIcons()
{
	uint8 count = CountPanelKeys();
	if(count < 2) return;

	int32 n = (Position() <= 0 ? 0 : CountVisibleItems(0, Position()));
	for(uint8 k = 0; k < count; k++)
	{
		switch(GetNavButtonIcon((int32)k))
		{
			case LBK_ICON_LEFT:
				if(n > 0)
					ShowNavButton(k);
				else
					HideNavButton(k);
				break;

			case LBK_ICON_OK:
				if(CurrentSelection() != NULL)
					ShowNavButton(k);
				else
					HideNavButton(k);
				break;

			case LBK_ICON_RIGHT:
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
LBMenuView::ItemInvoked(LBMenuItem *item)
{
	if(fAutoStandBack) StandBack();
}


void
LBMenuView::Attached()
{
	LBMenuItem *item;

	LBPageView::Attached();

	uint8 count = CountPanelKeys();
	switch(count)
	{
		case 2:
			// TODO: orientation, etc.
			SetNavButtonIcon(0, LBK_ICON_LEFT);
			SetNavButtonIcon(1, LBK_ICON_RIGHT);
			break;

		default:
			// TODO: orientation, etc.
			if(count < 3 || count > LBK_KEY_MAXIMUM_NUMBER) break;
			SetNavButtonIcon(count - 3, LBK_ICON_LEFT);
			SetNavButtonIcon(count - 2, LBK_ICON_OK);
			SetNavButtonIcon(count - 1, LBK_ICON_RIGHT);
	}

	for(int32 k = 0; k < CountItems(); k++)
	{
		item = cast_as(ItemAt(k), LBMenuItem);
		item->SetTarget(this);
	}
}


BRect
LBMenuView::ItemLabelBounds() const
{
	BRect r = Bounds();
	r.bottom = r.top + 13;
	return r;
}


BRect
LBMenuView::ItemIconBounds() const
{
	BRect r = Bounds();
	r.top += 14;
	return r;
}


int32
LBMenuView::VisibleItemsCountMax() const
{
	return(Bounds().Width() / 40);
}


void
LBMenuView::PositionChanged(int32 pos, int32 old)
{
	Invalidate(Bounds());
	RefreshNavButtonIcons();
}


void
LBMenuView::OffsetChanged(int32 offset, int32 old)
{
	Invalidate(Bounds());
}


void
LBMenuView::ScopeChanged()
{
	RefreshNavButtonIcons();
}


void
LBMenuView::SetAutoStandBack(bool state)
{
	fAutoStandBack = state;
}

