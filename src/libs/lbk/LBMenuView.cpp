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

// Comment the line below out when it's SURE to use it
#if LBK_KEY_TYPICAL_NUMBER < 2
#error "LBMenuView: Usually, it's useless when number of keys less than 2 !!!"
#endif


LBMenuView::LBMenuView(const char *name)
	: LBPageView(name),
	  LBScopeHandler(),
	  fAutoStandBack(true)
{
#if LBK_KEY_TYPICAL_NUMBER == 2
	SetNavButtonIcon(0, LBK_ICON_LEFT);
	SetNavButtonIcon(1, LBK_ICON_RIGHT);
#elif LBK_KEY_TYPICAL_NUMBER >= 3
	SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 3, LBK_ICON_LEFT);
	SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 2, LBK_ICON_OK);
	SetNavButtonIcon(LBK_KEY_TYPICAL_NUMBER - 1, LBK_ICON_RIGHT);
#endif
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

	if(clicks != 0xff) return;
	if(IsNavButtonHidden(key)) return;

	int32 pos = -1;
	switch(GetNavButtonIcon((int32)key))
	{
		case LBK_ICON_LEFT:
			FirstVisibleItem(pos);
			break;

		case LBK_ICON_RIGHT:
			LastVisibleItem(pos);
			break;

		default:
			return;
	}
	SetPosition(pos);
}

void
LBMenuView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	if(IsNavButtonHidden(key)) return;

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(btnIcon == LBK_ICON_NONE) return;

#if LBK_KEY_TYPICAL_NUMBER >= 3
	if(clicks == 1 && btnIcon == LBK_ICON_OK)
#else // LBK_KEY_TYPICAL_NUMBER < 3
	if((clicks == 1 && btnIcon == LBK_ICON_OK) ||
	   ((clicks > 1 && clicks != 0xff) && btnIcon == LBK_ICON_LEFT))
#endif
	{
		LBMenuItem *curItem = CurrentSelection();
		if(curItem != NULL)
		{
			if(curItem->Invoke() == B_OK)
				ItemInvoked(curItem);
		}
		return;
	}
	if(clicks > 1) return;

	switch(btnIcon)
	{
		case LBK_ICON_LEFT:
			RollDown();
			break;

		case LBK_ICON_RIGHT:
			RollUp();
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
	int32 n = (Position() <= 0 ? 0 : CountVisibleItems(0, Position()));

	// LEFT
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

	// RIGHT
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
LBMenuView::ItemInvoked(LBMenuItem *item)
{
	if(fAutoStandBack) StandBack();
}


void
LBMenuView::Attached()
{
	LBMenuItem *item;

	LBPageView::Attached();

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

