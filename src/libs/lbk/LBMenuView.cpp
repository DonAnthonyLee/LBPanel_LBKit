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
	  fSelected(-1),
	  fOffset(0)
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
	LBMenuItem *item;

	while((item = (LBMenuItem*)fItems.RemoveItem((int32)0)) != NULL)
	{
		item->fMenuView = NULL;
		delete item;
	}
}


bool
LBMenuView::AddItem(LBMenuItem *item)
{
	return AddItem(item, fItems.CountItems());
}


bool
LBMenuView::AddItem(LBMenuItem *item, int32 index)
{
	if(item == NULL || item->fMenuView != NULL) return false;
	if(fItems.AddItem(item, index) == false) return false;
	item->fMenuView = this;

	// TODO: update fOffset, fSelect, etc.

	return true;
}


bool
LBMenuView::RemoveItem(LBMenuItem *item)
{
	if(item == NULL || item->fMenuView != this) return false;
	return(RemoveItem(fItems.IndexOf(item)) != NULL);
}


LBMenuItem*
LBMenuView::RemoveItem(int32 index)
{
	LBMenuItem *item = ItemAt(index);

	if(item == NULL || fItems.RemoveItem(item) == false) return NULL;
	item->fMenuView = NULL;

	// TODO: update fOffset, fSelect, etc.

	return item;
}


LBMenuItem*
LBMenuView::ItemAt(int32 index) const
{
	return((LBMenuItem*)fItems.ItemAt(index));
}


int32
LBMenuView::CountItems() const
{
	return fItems.CountItems();
}


int32
LBMenuView::IndexOf(LBMenuItem *item) const
{
	return fItems.IndexOf(item);
}


LBMenuItem*
LBMenuView::FindItem(uint32 command) const
{
	LBMenuItem *item;

	for(int32 k = 0; k < fItems.CountItems(); k++)
	{
		item = ItemAt(k);
		if(item->Command() == command) return item;
	}

	return NULL; 
}


LBMenuItem*
LBMenuView::CurrentSelection() const
{
	LBMenuItem *item = ItemAt(fSelected);
	if(item == NULL || item->IsHidden()) return NULL;
	return item;
}


void
LBMenuView::ShowItem(int32 index)
{
	LBMenuItem *item = ItemAt(index);
	if(item == NULL || item->IsHidden() == false) return;
	item->fHidden = false;

	// TODO: update fOffset, fSelect, etc.
}


void
LBMenuView::HideItem(int32 index)
{
	LBMenuItem *item = ItemAt(index);
	if(item == NULL || item->IsHidden()) return;
	item->fHidden = true;

	// TODO: update fOffset, fSelect, etc.
}


int32
LBMenuView::CountVisibleItems(int32 fromIndex, int32 n) const
{
	int32 count = 0;
	LBMenuItem *item;

	if(fromIndex < 0) return 0;
	if(n < 0) n = CountItems() - fromIndex;

	for(int32 k = 0; k < n; k++)
	{
		item = ItemAt(fromIndex + k);
		if(item == NULL) break;
		if(item->IsHidden()) continue;
		count++;
	}

	return count;
}


LBMenuItem*
LBMenuView::PrevVisibleItem(int32 &index) const
{
	int32 k = index;
	LBMenuItem *item = ItemAt(--k);

	while(!(item == NULL || item->IsHidden() == false)) item = ItemAt(--k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBMenuItem*
LBMenuView::NextVisibleItem(int32 &index) const
{
	int32 k = index;
	LBMenuItem *item = ItemAt(++k);

	while(!(item == NULL || item->IsHidden() == false)) item = ItemAt(++k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBMenuItem*
LBMenuView::FirstVisibleItem(int32 &index) const
{
	LBMenuItem *item;

	for(int32 k = 0; k < CountItems(); k++)
	{
		item = ItemAt(k);
		if(item->IsHidden() == false)
		{
			index = k;
			return item;
		}
	}

	return NULL;
}


LBMenuItem*
LBMenuView::LastVisibleItem(int32 &index) const
{
	LBMenuItem *item;

	for(int32 k = CountItems() - 1; k >= 0; k--)
	{
		item = ItemAt(k);
		if(item->IsHidden() == false)
		{
			index = k;
			return item;
		}
	}

	return NULL;
}


void
LBMenuView::Draw(BRect rect)
{
	LBPageView::Draw(rect);
	if(fItems.CountItems() == 0) return;

	uint16 w;
	BRect r;
	LBMenuItem *curItem = CurrentSelection();

	// item label
	r = Bounds();
	r.bottom = r.top + 13;
	if(!(curItem == NULL || curItem->Label() == NULL) && r.Intersects(rect))
	{
		SetFontSize(12);
		w = StringWidth(curItem->Label());
		DrawString(curItem->Label(), BPoint(r.Center().x - w / 2.f, 1));
	}

	// item icons
	r = Bounds();
	r.top += 14;
	if(r.Intersects(rect) == false) return;

	int32 nMax = r.Width() / 40;
	r.right = r.left + r.Width() / (float)nMax - 1.f;

	LBMenuItem *aItem;
	int32 index = fOffset;
	for(int32 k = 0; k < nMax; k++, index++)
	{
		aItem = ItemAt(index);
		while(!(aItem == NULL || aItem->IsHidden() == false)) aItem = ItemAt(++index);
		if(aItem == NULL) break;

		if(r.Intersects(rect))
		{
			if(aItem == curItem)
			{
				FillRect(r & rect);

				const lbk_icon *icon = lbk_get_icon_data(aItem->Icon());
				if(icon != NULL)
				{
					lbk_icon icon_inverse;
					icon_inverse.type = icon->type;
					for(size_t k = 0; k < sizeof(icon_inverse.data); k++)
						icon_inverse.data[k] = ~(icon->data[k]);
					DrawIcon(&icon_inverse, r.Center() - BPoint(15, 15));
				}
			}
			else if(aItem->Icon() != LBK_ICON_NONE)
			{
				DrawIcon(aItem->Icon(), r.Center() - BPoint(15, 15));
			}
		}

		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
LBMenuView::ResetOffsetIfNeeded()
{
	if(fSelected < 0)
	{
		fOffset = 0;
	}
	else if(fSelected < fOffset)
	{
		fOffset = fSelected;
	}
	else
	{
		int32 nMax = Bounds().Width() / 40;
		LBMenuItem *aItem;
		int32 lastIndex = fOffset;
		for(int32 k = 0; k < nMax; k++, lastIndex++)
		{
			aItem = ItemAt(lastIndex);
			while(!(aItem == NULL || aItem->IsHidden() == false)) aItem = ItemAt(++lastIndex);
			if(aItem == NULL) break;
			if(k == nMax - 1) break;
		}

		if(fSelected > lastIndex)
		{
			int32 n = CountVisibleItems(lastIndex + 1, fSelected - lastIndex);
			while(n-- > 0) NextVisibleItem(fOffset);
		}
	}
}


void
LBMenuView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	if(clicks != 0xff) return;
	if(IsNavButtonHidden(key)) return;

	lbk_icon_id btnIcon = GetNavButtonIcon((int32)key);

	int32 saveSelected = fSelected;
	switch(btnIcon)
	{
		case LBK_ICON_LEFT:
			FirstVisibleItem(fSelected);
			break;

		case LBK_ICON_RIGHT:
			LastVisibleItem(fSelected);
			break;

		default:
			break;
	}
	if(saveSelected == fSelected) return;

	ResetOffsetIfNeeded();
	RefreshNavButtonIcons();
	InvalidRect(Bounds());
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
			curItem->Invoke();
			ItemInvoked(curItem);
		}
		return;
	}
	if(clicks > 1) return;

	int32 saveSelected = fSelected;
	switch(btnIcon)
	{
		case LBK_ICON_LEFT:
			PrevVisibleItem(fSelected);
			break;

		case LBK_ICON_RIGHT:
			NextVisibleItem(fSelected);
			break;

		default:
			break;
	}
	if(saveSelected == fSelected) return;

	ResetOffsetIfNeeded();
	RefreshNavButtonIcons();
	InvalidRect(Bounds());
}


void
LBMenuView::StandIn()
{
	LBPageView::StandIn();

	if(fSelected < 0)
	{
		FirstVisibleItem(fSelected);
		fOffset = max_c(0, fSelected);

		RefreshNavButtonIcons();
	}
}


void
LBMenuView::Activated(bool state)
{
	LBPageView::Activated(state);

	if(state && fSelected < 0)
	{
		FirstVisibleItem(fSelected);
		fOffset = max_c(0, fSelected);

		RefreshNavButtonIcons();
	}
}


void
LBMenuView::RefreshNavButtonIcons()
{
	int32 n = (fSelected <= 0 ? 0 : CountVisibleItems(0, fSelected));

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
	n = CountVisibleItems(fSelected + 1, -1);
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
	StandBack();
}

