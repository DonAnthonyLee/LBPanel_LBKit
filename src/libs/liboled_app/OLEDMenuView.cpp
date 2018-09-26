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
 * File: OLEDMenuView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "OLEDConfig.h"
#include <OLEDMenuView.h>

// Comment the line below out when it's SURE to use it
#if OLED_BUTTONS_NUM < 2
#error "OLEDMenuView: Usually, it's useless when number of buttons less than 2 !!!"
#endif


OLEDMenuView::OLEDMenuView(const char *name)
	: OLEDPageView(name),
	  fSelected(-1),
	  fOffset(0)
{
#if OLED_BUTTONS_NUM == 2
	SetNavButtonIcon(0, OLED_ICON_LEFT);
	SetNavButtonIcon(1, OLED_ICON_RIGHT);
#elif OLED_BUTTONS_NUM >= 3
	SetNavButtonIcon(OLED_BUTTONS_NUM - 3, OLED_ICON_LEFT);
	SetNavButtonIcon(OLED_BUTTONS_NUM - 2, OLED_ICON_OK);
	SetNavButtonIcon(OLED_BUTTONS_NUM - 1, OLED_ICON_RIGHT);
#endif
}


OLEDMenuView::~OLEDMenuView()
{
	OLEDMenuItem *item;

	while((item = (OLEDMenuItem*)fItems.RemoveItem((int32)0)) != NULL)
	{
		item->fMenuView = NULL;
		delete item;
	}
}


bool
OLEDMenuView::AddItem(OLEDMenuItem *item)
{
	return AddItem(item, fItems.CountItems());
}


bool
OLEDMenuView::AddItem(OLEDMenuItem *item, int32 index)
{
	if(item == NULL || item->fMenuView != NULL) return false;
	if(fItems.AddItem(item, index) == false) return false;
	item->fMenuView = this;

	// TODO: update fOffset, fSelect, etc.

	return true;
}


bool
OLEDMenuView::RemoveItem(OLEDMenuItem *item)
{
	if(item == NULL || item->fMenuView != this) return false;
	return(RemoveItem(fItems.IndexOf(item)) != NULL);
}


OLEDMenuItem*
OLEDMenuView::RemoveItem(int32 index)
{
	OLEDMenuItem *item = ItemAt(index);

	if(item == NULL || fItems.RemoveItem(item) == false) return NULL;
	item->fMenuView = NULL;

	// TODO: update fOffset, fSelect, etc.

	return item;
}


OLEDMenuItem*
OLEDMenuView::ItemAt(int32 index) const
{
	return((OLEDMenuItem*)fItems.ItemAt(index));
}


int32
OLEDMenuView::CountItems() const
{
	return fItems.CountItems();
}


int32
OLEDMenuView::IndexOf(OLEDMenuItem *item) const
{
	return fItems.IndexOf(item);
}


OLEDMenuItem*
OLEDMenuView::FindItem(uint32 command) const
{
	OLEDMenuItem *item;

	for(int32 k = 0; k < fItems.CountItems(); k++)
	{
		item = ItemAt(k);
		if(item->Command() == command) return item;
	}

	return NULL; 
}


OLEDMenuItem*
OLEDMenuView::CurrentSelection() const
{
	OLEDMenuItem *item = ItemAt(fSelected);
	if(item == NULL || item->IsHidden()) return NULL;
	return item;
}


void
OLEDMenuView::ShowItem(int32 index)
{
	OLEDMenuItem *item = ItemAt(index);
	if(item == NULL || item->IsHidden() == false) return;
	item->fHidden = false;

	// TODO: update fOffset, fSelect, etc.
}


void
OLEDMenuView::HideItem(int32 index)
{
	OLEDMenuItem *item = ItemAt(index);
	if(item == NULL || item->IsHidden()) return;
	item->fHidden = true;

	// TODO: update fOffset, fSelect, etc.
}


int32
OLEDMenuView::CountVisibleItems(int32 fromIndex, int32 n) const
{
	int32 count = 0;
	OLEDMenuItem *item;

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


OLEDMenuItem*
OLEDMenuView::PrevVisibleItem(int32 &index) const
{
	int32 k = index;
	OLEDMenuItem *item = ItemAt(--k);

	while(!(item == NULL || item->IsHidden() == false)) item = ItemAt(--k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


OLEDMenuItem*
OLEDMenuView::NextVisibleItem(int32 &index) const
{
	int32 k = index;
	OLEDMenuItem *item = ItemAt(++k);

	while(!(item == NULL || item->IsHidden() == false)) item = ItemAt(++k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


OLEDMenuItem*
OLEDMenuView::FirstVisibleItem(int32 &index) const
{
	OLEDMenuItem *item;

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


OLEDMenuItem*
OLEDMenuView::LastVisibleItem(int32 &index) const
{
	OLEDMenuItem *item;

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
OLEDMenuView::Draw(BRect rect)
{
	OLEDPageView::Draw(rect);
	if(fItems.CountItems() == 0) return;

	uint16 w;
	BRect r;
	OLEDMenuItem *curItem = CurrentSelection();

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

	OLEDMenuItem *aItem;
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

				const oled_icon *icon = oled_get_icon_data(aItem->Icon());
				if(icon != NULL)
				{
					oled_icon icon_inverse;
					icon_inverse.type = icon->type;
					for(size_t k = 0; k < sizeof(icon_inverse.data); k++)
						icon_inverse.data[k] = ~(icon->data[k]);
					DrawIcon(&icon_inverse, r.Center() - BPoint(15, 15));
				}
			}
			else if(aItem->Icon() != OLED_ICON_NONE)
			{
				DrawIcon(aItem->Icon(), r.Center() - BPoint(15, 15));
			}
		}

		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
OLEDMenuView::ResetOffsetIfNeeded()
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
		OLEDMenuItem *aItem;
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
OLEDMenuView::KeyDown(uint8 key, uint8 clicks)
{
	OLEDPageView::KeyDown(key, clicks);

	if(clicks != 0xff) return;
	if(IsNavButtonHidden(key)) return;

	oled_icon_id btnIcon = GetNavButtonIcon((int32)key);

	int32 saveSelected = fSelected;
	switch(btnIcon)
	{
		case OLED_ICON_LEFT:
			FirstVisibleItem(fSelected);
			break;

		case OLED_ICON_RIGHT:
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
OLEDMenuView::KeyUp(uint8 key, uint8 clicks)
{
	OLEDPageView::KeyUp(key, clicks);

	if(IsNavButtonHidden(key)) return;

	oled_icon_id btnIcon = GetNavButtonIcon((int32)key);
	if(btnIcon == OLED_ICON_NONE) return;

#if OLED_BUTTONS_NUM >= 3
	if(clicks == 1 && btnIcon == OLED_ICON_OK)
#else // OLED_BUTTONS_NUM < 3
	if((clicks == 1 && btnIcon == OLED_ICON_OK) ||
	   ((clicks > 1 && clicks != 0xff) && btnIcon == OLED_ICON_LEFT))
#endif
	{
		OLEDMenuItem *curItem = CurrentSelection();
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
		case OLED_ICON_LEFT:
			PrevVisibleItem(fSelected);
			break;

		case OLED_ICON_RIGHT:
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
OLEDMenuView::StandIn()
{
	OLEDPageView::StandIn();

	if(fSelected < 0)
	{
		FirstVisibleItem(fSelected);
		fOffset = max_c(0, fSelected);

		RefreshNavButtonIcons();
	}
}


void
OLEDMenuView::Activated(bool state)
{
	OLEDPageView::Activated(state);

	if(state && fSelected < 0)
	{
		FirstVisibleItem(fSelected);
		fOffset = max_c(0, fSelected);

		RefreshNavButtonIcons();
	}
}


void
OLEDMenuView::RefreshNavButtonIcons()
{
	int32 n = (fSelected <= 0 ? 0 : CountVisibleItems(0, fSelected));

	// LEFT
	if(n > 0)
	{
#if OLED_BUTTONS_NUM == 2
		ShowNavButton(0);
#elif OLED_BUTTONS_NUM >= 3
		ShowNavButton(OLED_BUTTONS_NUM - 3);
#endif
	}
	else
	{
#if OLED_BUTTONS_NUM == 2
		HideNavButton(0);
#elif OLED_BUTTONS_NUM >= 3
		HideNavButton(OLED_BUTTONS_NUM - 3);
#endif
	}

	// OK
#if OLED_BUTTONS_NUM >= 3
	if(CurrentSelection() != NULL)
		ShowNavButton(OLED_BUTTONS_NUM - 2);
	else
		HideNavButton(OLED_BUTTONS_NUM - 2);
#endif

	// RIGHT
	n = CountVisibleItems(fSelected + 1, -1);
	if(n > 0)
	{
#if OLED_BUTTONS_NUM == 2
		ShowNavButton(1);
#elif OLED_BUTTONS_NUM >= 3
		ShowNavButton(OLED_BUTTONS_NUM - 1);
#endif
	}
	else
	{
#if OLED_BUTTONS_NUM == 2
		HideNavButton(1);
#elif OLED_BUTTONS_NUM >= 3
		HideNavButton(OLED_BUTTONS_NUM - 1);
#endif
	}
}


void
OLEDMenuView::ItemInvoked(OLEDMenuItem *item)
{
	StandBack();
}

