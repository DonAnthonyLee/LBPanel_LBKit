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

#include <OLEDMenuView.h>


OLEDMenuView::OLEDMenuView(const char *name)
	: OLEDPageView(name),
	  fSelected(-1)
{
	// TODO
}


OLEDMenuView::~OLEDMenuView()
{
	OLEDMenuItem *item;
	while((item = (OLEDMenuItem*)fItems.RemoveItem((int32)0)) != NULL) delete item;

	// TODO
}


bool
OLEDMenuView::AddItem(OLEDMenuItem *item)
{
	return AddItem(item, fItems.CountItems());
}


bool
OLEDMenuView::AddItem(OLEDMenuItem *item, int32 index)
{
	// TODO
	return false;
}


bool
OLEDMenuView::RemoveItem(OLEDMenuItem *item)
{
	// TODO
	return false;
}


OLEDMenuItem*
OLEDMenuView::RemoveItem(int32 index)
{
	// TODO
	return NULL;
}


OLEDMenuItem*
OLEDMenuView::ItemAt(int32 index)
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
		item = (OLEDMenuItem*)fItems.ItemAt(k);
		if(item->Command() == command) return item;
	}

	return NULL; 
}


OLEDMenuItem*
OLEDMenuView::CurrentSelection() const
{
	OLEDMenuItem *item = (OLEDMenuItem*)fItems.ItemAt(fSelected);
	if(item == NULL || item->IsHidden()) return NULL;
	return item;
}


void
OLEDMenuView::Draw(BRect rect)
{
	OLEDPageView::Draw(rect);

	// TODO
}


void
OLEDMenuView::KeyDown(uint8 key, uint8 clicks)
{
	OLEDPageView::KeyDown(key, clicks);

	// TODO
}


void
OLEDMenuView::KeyUp(uint8 key, uint8 clicks)
{
	OLEDPageView::KeyUp(key, clicks);

	// TODO
}


void
OLEDMenuView::ItemInvoked(OLEDMenuItem *item)
{
	StandBack();
}

