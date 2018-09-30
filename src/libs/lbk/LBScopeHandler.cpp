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
 * File: LBScopeHandler.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBScopeHandler.h>


LBScopeHandler::LBScopeHandler(const char *name)
	: fOffset(0), fPos(0)
{
}


LBScopeHandler::~LBScopeHandler()
{
	LBScopeItem *item;

	while((item = (LBScopeItem*)fItems.RemoveItem((int32)0)) != NULL)
	{
		item->fHandler = NULL;
		delete item;
	}
}


bool
LBScopeHandler::AddItem(LBScopeItem *item)
{
	return AddItem(item, fItems.CountItems());
}


bool
LBScopeHandler::AddItem(LBScopeItem *item, int32 index)
{
	if(item == NULL || item->fHandler != NULL) return false;
	if(fItems.AddItem(item, index) == false) return false;
	item->fHandler = this;

	// TODO

	return true;
}


bool
LBScopeHandler::RemoveItem(LBScopeItem *item)
{
	if(item == NULL || item->fHandler != this) return false;
	return(RemoveItem(fItems.IndexOf(item)) != NULL);
}


LBScopeItem*
LBScopeHandler::RemoveItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);

	if(item == NULL || fItems.RemoveItem(item) == false) return NULL;
	item->fHandler = NULL;

	// TODO

	return item;
}


LBScopeItem*
LBScopeHandler::ItemAt(int32 index) const
{
	return((LBScopeItem*)fItems.ItemAt(index));
}


int32
LBScopeHandler::CountItems() const
{
	return fItems.CountItems();
}


int32
LBScopeHandler::IndexOf(LBScopeItem *item) const
{
	return fItems.IndexOf(item);
}


bool
LBScopeHandler::SwapItems(int32 indexA, int32 indexB)
{
	bool retVal = false;

	do
	{
		if((retVal = fItems.SwapItems(indexA, indexB)) == false) break;

		// TODO

	} while(false);

	return retVal;
}


bool
LBScopeHandler::MoveItem(int32 fromIndex, int32 toIndex)
{
	bool retVal = false;

	do
	{
		if((retVal = fItems.MoveItem(fromIndex, toIndex)) == false) break;

		// TODO

	} while(false);

	return retVal;
}


void
LBScopeHandler::SortItems(int (*cmpFunc)(const LBScopeItem**, const LBScopeItem**))
{
	if(cmpFunc == NULL) return;

	fItems.SortItems((int (*)(const void*, const void*))cmpFunc);

	// TODO
}


void
LBScopeHandler::DoForEach(bool (*func)(LBScopeItem *item))
{
	fItems.DoForEach((bool (*)(void*))func);
}


int32
LBScopeHandler::Position() const
{
	return fPos;
}


void
LBScopeHandler::SetPosition(int32 pos)
{
	if(fPos != pos)
	{
		int32 old = fPos;
		fPos = pos;

		// TODO
		PositionChanged(fPos, old);
	}
}


void
LBScopeHandler::PositionChanged(int32 pos, int32 old)
{
	// EMPTY
}


int32
LBScopeHandler::Offset() const
{
	return fOffset;
}


void
LBScopeHandler::SetOffset(int32 offset)
{
	if(fOffset != offset)
	{
		int32 old = fOffset;
		fOffset = offset;

		// TODO
		OffsetChanged(fOffset, old);
	}
}


void
LBScopeHandler::OffsetChanged(int32 offset, int32 old)
{
	// EMPTY
}


void
LBScopeHandler::RollUp()
{
	// TODO
}


void
LBScopeHandler::RollDown()
{
	// TODO
}


void
LBScopeHandler::ShowItem(LBScopeItem *item)
{
	if(item == NULL || item->fHandler != this) return;
	ShowItem(fItems.IndexOf(item));
}


void
LBScopeHandler::ShowItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);
	if(item == NULL || item->IsVisible()) return;
	item->fVisible = true;

	// TODO
}


void
LBScopeHandler::HideItem(LBScopeItem *item)
{
	if(item == NULL || item->fHandler != this) return;
	HideItem(fItems.IndexOf(item));
}


void
LBScopeHandler::HideItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);
	if(item == NULL || item->IsVisible() == false) return;
	item->fVisible = false;

	// TODO
}


bool
LBScopeHandler::IsItemVisible(const LBScopeItem *item) const
{
	// TODO
	return false;
}


void
LBScopeHandler::Expand()
{
	// TODO
}


void
LBScopeHandler::Collapse()
{
	// TODO
}


void
LBScopeHandler::Show()
{
	// TODO
	LBScopeItem::Show();
}


void
LBScopeHandler::Hide()
{
	// TODO
	LBScopeItem::Hide();
}


int32
LBScopeHandler::VisibleItemsCountMax() const
{
	return 0;
}


int32
LBScopeHandler::CountVisibleItems(int32 fromIndex, int32 n) const
{
	int32 count = 0;
	LBScopeItem *item;

	if(fromIndex < 0) return 0;
	if(n < 0) n = CountItems() - fromIndex;

	for(int32 k = 0; k < n; k++)
	{
		item = ItemAt(fromIndex + k);
		if(item == NULL) break;
		if(item->IsVisible() == false) continue;
		count++;
	}

	return count;
}


LBScopeItem*
LBScopeHandler::PrevVisibleItem(int32 &index) const
{
	int32 k = index;
	LBScopeItem *item = ItemAt(--k);

	while(!(item == NULL || item->IsVisible())) item = ItemAt(--k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBScopeItem*
LBScopeHandler::NextVisibleItem(int32 &index) const
{
	int32 k = index;
	LBScopeItem *item = ItemAt(++k);

	while(!(item == NULL || item->IsVisible())) item = ItemAt(++k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBScopeItem*
LBScopeHandler::FirstVisibleItem(int32 &index) const
{
	LBScopeItem *item;

	for(int32 k = 0; k < CountItems(); k++)
	{
		item = ItemAt(k);
		if(item->IsVisible())
		{
			index = k;
			return item;
		}
	}

	return NULL;
}


LBScopeItem*
LBScopeHandler::LastVisibleItem(int32 &index) const
{
	LBScopeItem *item;

	for(int32 k = CountItems() - 1; k >= 0; k--)
	{
		item = ItemAt(k);
		if(item->IsVisible())
		{
			index = k;
			return item;
		}
	}

	return NULL;
}


void
LBScopeHandler::ResetOffsetIfNeeded()
{
	if(fPos < 0)
	{
		this->SetOffset(0);
	}
	else if(fPos < fOffset)
	{
		this->SetOffset(fPos);
	}
	else
	{
		int32 nMax = this->VisibleItemsCountMax();
		LBScopeItem *aItem;
		int32 lastIndex = fOffset;
		for(int32 k = 0; k < nMax; k++, lastIndex++)
		{
			aItem = ItemAt(lastIndex);
			while(!(aItem == NULL || aItem->IsVisible())) aItem = ItemAt(++lastIndex);
			if(aItem == NULL) break;
			if(k == nMax - 1) break;
		}

		if(fPos > lastIndex)
		{
			int32 n = CountVisibleItems(lastIndex + 1, fPos - lastIndex);
			int32 offset = fOffset;
			while(n-- > 0) NextVisibleItem(offset);
			this->SetOffset(offset);
		}
	}
}

