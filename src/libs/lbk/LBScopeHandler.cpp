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
	: fOffset(0), fPos(-1), fExpanded(true)
{
}


LBScopeHandler::~LBScopeHandler()
{
	LBScopeItem *item;

	while((item = (LBScopeItem*)fItems.RemoveItem((int32)0)) != NULL)
	{
		item->fScopeHandler = NULL;
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
	if(index < 0 || item == NULL || item->fScopeHandler != NULL) return false;

	if(fItems.AddItem(item, index) == false) return false;
	item->fScopeHandler = this;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;
	bool scopeChanged = false;

	// update offset and position if necessary
	if(index <= fOffset) fOffset++;
	if(index <= fPos) fPos++;

	if(item->fVisible) // use default behavior if item is visible
		scopeChanged = ReadjustWhenItemShown(index, fOffset, fPos);

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();

	return true;
}


bool
LBScopeHandler::RemoveItem(LBScopeItem *item)
{
	if(item == NULL || item->fScopeHandler != this) return false;
	return(RemoveItem(fItems.IndexOf(item)) != NULL);
}


LBScopeItem*
LBScopeHandler::RemoveItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);
	if(item == NULL) return NULL;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;
	bool scopeChanged = false;

	if(item->fVisible) // use default behavior if item is visible
	{
		item->fVisible = false;
		scopeChanged = ReadjustWhenItemHidden(index, fOffset, fPos);
		item->fVisible = true;
	}

	fItems.RemoveItem(index);
	item->fScopeHandler = NULL;

	if(index < fOffset) fOffset--;
	if(index < fPos) fPos--;

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();

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
LBScopeHandler::IndexOf(const LBScopeItem *item) const
{
	if(item == NULL) return -1;

	LBScopeItem *t = const_cast<LBScopeItem*>(item);
	return fItems.IndexOf(reinterpret_cast<void*>(t));
}


bool
LBScopeHandler::SwapItems(int32 indexA, int32 indexB)
{
	if(indexA < 0 || indexA >= fItems.CountItems()) return false;
	if(indexB < 0 || indexB >= fItems.CountItems()) return false;
	if(indexA == indexB) return true;

	int32 last = -1;
	LastVisibleItemAtScope(last);

	if(fItems.SwapItems(indexA, indexB) == false) return false;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;

	// use default behavior
	bool scopeChanged = ReadjustWhenItemsSwapped(indexA, indexB, last, fOffset, fPos);

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();

	return true;
}


bool
LBScopeHandler::MoveItem(int32 fromIndex, int32 toIndex)
{
	if(fromIndex < 0 || fromIndex >= fItems.CountItems()) return false;
	if(toIndex < 0 || toIndex >= fItems.CountItems()) return false;
	if(fromIndex == toIndex) return true;

	LBScopeItem *item = ItemAt(fromIndex);
	if(item == NULL) return false;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;
	bool scopeChanged = false;

	if(item->fVisible) // use default behavior if item is visible
	{
		item->fVisible = false;
		scopeChanged = ReadjustWhenItemHidden(fromIndex, fOffset, fPos);
		item->fVisible = true;
	}

	if(fItems.MoveItem(fromIndex, toIndex) == false)
	{
		fOffset = saveOffset;
		fPos = savePos;
		return false;
	}

	if(fromIndex < fOffset) fOffset--;
	if(fromIndex < fPos) fPos--;

	if(toIndex <= fOffset) fOffset++;
	if(toIndex <= fPos) fPos++;

	if(item->fVisible) // use default behavior if item is visible
	{
		if(ReadjustWhenItemShown(toIndex, fOffset, fPos))
			scopeChanged = true;
	}

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();

	return true;
}


void
LBScopeHandler::SortItems(int (*cmpFunc)(const LBScopeItem**, const LBScopeItem**))
{
	if(cmpFunc == NULL) return;

	fItems.SortItems((int (*)(const void*, const void*))cmpFunc);

	// Here we supposed that most of items changed, so it's unnecessary to keep the position.
	SetPosition(-1);

	// If nothing inside the scope, reset the offset.
	if(CountVisibleItemsAtScope(fOffset, -1) == 0) SetOffset(0);
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
	LBScopeItem *item = ItemAt(pos);
	if(item == NULL ? (pos >= 0) : (item->fVisible == false)) return;

	if(fPos != pos)
	{
		int32 saveOffset = fOffset;
		int32 savePos = fPos;

		fPos = pos;
		fOffset = ReadjustOffsetWhenPositionChanged(); // use default behavior

		if(fOffset != saveOffset)
			OffsetChanged(fOffset, saveOffset);

		PositionChanged(fPos, savePos);

		if(fOffset != saveOffset)
			ScopeChanged();
	}
}


void
LBScopeHandler::PositionChanged(int32 pos, int32 old)
{
	// EMPTY
}


void
LBScopeHandler::ScopeChanged()
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
		int32 saveOffset = fOffset;
		int32 savePos = fPos;

		fOffset = offset;
		if(fPos >= 0)
		{
			if(fPos < fOffset)
			{
				fPos = fOffset - 1;
				if(NextVisibleItemAtScope(fPos) == NULL) fPos = -1;
			}
			else
			{
				int32 last = -1;
				LastVisibleItemAtScope(last);
				if(fPos > last) fPos = last;
			}
		}

		OffsetChanged(fOffset, saveOffset);

		if(savePos != fPos)
			PositionChanged(fPos, savePos);
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
	int32 pos = fPos;

	if(NextVisibleItem(pos) != NULL) SetPosition(pos);
}


void
LBScopeHandler::RollDown()
{
	int32 pos = fPos;

	if(PrevVisibleItem(pos) != NULL) SetPosition(pos);
}


void
LBScopeHandler::ShowItem(LBScopeItem *item)
{
	if(item == NULL || item->fScopeHandler != this) return;
	ShowItem(fItems.IndexOf(item));
}


void
LBScopeHandler::ShowItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);
	if(item == NULL || item->fVisible) return;
	item->fVisible = true;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;

	// use default behavior
	bool scopeChanged = ReadjustWhenItemShown(index, fOffset, fPos);

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();
}


void
LBScopeHandler::HideItem(LBScopeItem *item)
{
	if(item == NULL || item->fScopeHandler != this) return;
	HideItem(fItems.IndexOf(item));
}


void
LBScopeHandler::HideItem(int32 index)
{
	LBScopeItem *item = ItemAt(index);
	if(item == NULL || item->fVisible == false) return;
	item->fVisible = false;

	int32 saveOffset = fOffset;
	int32 savePos = fPos;

	// use default behavior
	bool scopeChanged = ReadjustWhenItemHidden(index, fOffset, fPos);

	if(saveOffset != fOffset)
		OffsetChanged(fOffset, saveOffset);

	if(savePos != fPos)
		PositionChanged(fPos, savePos);

	if(scopeChanged)
		ScopeChanged();
}


bool
LBScopeHandler::IsItemVisible(const LBScopeItem *item) const
{
	int32 index = IndexOf(item);
	if(index < 0 || item->fVisible == false) return false;
	if(fExpanded == false) return false;
	if(fVisible == false) return false;
	if(CountVisibleItemsAtScope(index, 1) == 0) return false;
	if(ScopeHandler() != NULL) return ScopeHandler()->IsItemVisible(this);
	return true;
}


bool
LBScopeHandler::IsExpanded() const
{
	return fExpanded;
}


void
LBScopeHandler::Expand()
{
	if(fExpanded) return;
	fExpanded = true;
	ScopeChanged();
}


void
LBScopeHandler::Collapse()
{
	if(fExpanded == false) return;
	fExpanded = false;
	ScopeChanged();
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
		if(item->fVisible == false) continue;
		count++;
	}

	return count;
}


LBScopeItem*
LBScopeHandler::PrevVisibleItem(int32 &index) const
{
	int32 k = index;
	LBScopeItem *item = ItemAt(--k);

	while(!(item == NULL || item->fVisible)) item = ItemAt(--k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBScopeItem*
LBScopeHandler::NextVisibleItem(int32 &index) const
{
	int32 k = index;
	LBScopeItem *item = ItemAt(++k);

	while(!(item == NULL || item->fVisible)) item = ItemAt(++k);
	if(item == NULL) return NULL;

	index = k;
	return item;
}


LBScopeItem*
LBScopeHandler::FirstVisibleItem(int32 &index, int32 fromOffset) const
{
	LBScopeItem *item;

	for(int32 k = fromOffset; k < CountItems(); k++)
	{
		item = ItemAt(k);
		if(item->fVisible)
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
		if(item->fVisible)
		{
			index = k;
			return item;
		}
	}

	return NULL;
}


int32
LBScopeHandler::CountVisibleItemsAtScope(int32 fromIndex, int32 n) const
{
	int32 count = 0, retVal = 0;
	LBScopeItem *item;
	int32 nMax = this->VisibleItemsCountMax();

	if(fromIndex < 0 || nMax < 1) return 0;
	if(n < 0) n = CountItems() - fromIndex;

	for(int32 k = fOffset; k < fromIndex + n && count < nMax; k++)
	{
		item = ItemAt(k);
		if(item == NULL) break;
		if(item->fVisible == false) continue;
		count++;
		if(k >= fromIndex) retVal++;
	}

	return retVal;
}


LBScopeItem*
LBScopeHandler::FirstVisibleItemAtScope(int32 &index, int32 fromIndex) const
{
	int32 t = (fromIndex < 0 ? fOffset : fromIndex) - 1;
	LBScopeItem *item = NextVisibleItemAtScope(t);
	if(item != NULL) index = t;
	return item;
}


LBScopeItem*
LBScopeHandler::LastVisibleItemAtScope(int32 &index) const
{
	LBScopeItem *item = NULL;
	int32 nMax = this->VisibleItemsCountMax();
	int32 t = fOffset;

	for(int32 k = 0; k < nMax; k++, t++)
	{
		LBScopeItem *aItem = ItemAt(t);
		while(!(aItem == NULL || aItem->fVisible)) aItem = ItemAt(++t);
		if(aItem == NULL) break;
		item = aItem;
	}

	if(item != NULL)
		index = fItems.IndexOf(item);

	return item;
}


LBScopeItem*
LBScopeHandler::PrevVisibleItemAtScope(int32 &index) const
{
	LBScopeItem *item = NULL;
	int32 nMax = this->VisibleItemsCountMax();
	int32 t = fOffset;

	for(int32 k = 0; k < nMax && t < index; k++, t++)
	{
		LBScopeItem *aItem = ItemAt(t);
		while(!(aItem == NULL || aItem->fVisible)) aItem = ItemAt(++t);
		if(aItem == NULL) break;
		item = aItem;
	}

	if(item != NULL)
		index = fItems.IndexOf(item);

	return item;
}


LBScopeItem*
LBScopeHandler::NextVisibleItemAtScope(int32 &index) const
{
	int32 nMax = this->VisibleItemsCountMax();
	int32 t = fOffset;

	for(int32 k = 0; k < nMax; k++, t++)
	{
		LBScopeItem *aItem = ItemAt(t);
		while(!(aItem == NULL || aItem->fVisible)) aItem = ItemAt(++t);
		if(aItem == NULL) break;
		if(t > index)
		{
			index = t;
			return aItem;
		}
	}

	return NULL;
}


int32
LBScopeHandler::ReadjustOffsetWhenPositionChanged() const
{
	if(fPos < 0) return fOffset; // remain original value
	if(fPos < fOffset) return fPos; // make it start from "fPos"

	int32 last = -1;
	LastVisibleItemAtScope(last);

	if(last < 0) return fPos;
	if(fPos <= last) return fOffset;

	// fPos > last, keep "fPos" at last position of scope
	int32 n = CountVisibleItems(last + 1, fPos - last);
	int32 offset = fOffset;
	while(n-- > 0) NextVisibleItem(offset);
	return offset;
}


bool
LBScopeHandler::ReadjustWhenItemShown(int32 item_index, int32 &new_offset, int32 &new_pos) const
{
	int32 last = -1;
	LastVisibleItemAtScope(last);

	new_offset = Offset();
	new_pos = min_c(Position(), last);

	return(item_index >= Offset() && item_index <= last);
}


bool
LBScopeHandler::ReadjustWhenItemHidden(int32 item_index, int32 &new_offset, int32 &new_pos) const
{
	bool scopeChanged = false;

	new_offset = Offset();
	new_pos = Position();

	if(item_index >= Offset())
	{
		int32 nMax = this->VisibleItemsCountMax();
		if(CountVisibleItems(Offset(), item_index - Offset()) < nMax)
		{
			new_pos = -1;

			if(item_index == Offset() && item_index == CountItems() - 1)
				new_offset--;
			else
				LastVisibleItemAtScope(new_pos);

			scopeChanged = true;
		}
	}

	return scopeChanged;
}


bool
LBScopeHandler::ReadjustWhenItemsSwapped(int32 itemA_index, int32 itemB_index,
					 int32 last_index_at_scope_before,
					 int32 &new_offset, int32 &new_pos) const
{
	LBScopeItem *itemA = ItemAt(itemB_index);
	LBScopeItem *itemB = ItemAt(itemA_index);
	bool scopeChanged = false;

	new_offset = Offset();
	new_pos = Position();

	if((itemA_index >= Offset() && itemA_index <= last_index_at_scope_before) ||
	   (itemB_index >= Offset() && itemB_index <= last_index_at_scope_before))
	{
		if(itemA->fVisible != itemB->fVisible)
		{
			if((itemA->fVisible && itemA_index == Position()) ||
			   (itemB->fVisible && itemB_index == Position()))
			{
				if(NextVisibleItemAtScope(new_pos) == NULL)
				{
					if(PrevVisibleItemAtScope(new_pos) == NULL) new_pos = -1;
				}
			} 

			scopeChanged = true;
		}
	}

	if(scopeChanged == false) // recheck scope
	{
		int32 t = -1;
		LastVisibleItemAtScope(t);
		if(t != last_index_at_scope_before) scopeChanged = true;
	}

	return scopeChanged;
}

