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
 * File: LBListItem.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBListItem.h>
#include <lbk/LBListView.h>

#define ICON_IS_8x8(id)		((id) > LBK_ICON_ID_8x8_BEGIN && (id) < LBK_ICON_ID_8x8_END)


LBListItem::LBListItem()
	: LBScopeItem()
{
}


LBListItem::~LBListItem()
{
}


bool
LBListItem::IsHidden() const
{
	return(!IsVisible());
}


void
LBListItem::DrawString(const char *str, BRect r, int32 n)
{
	if(str == NULL || *str == 0 || r.IsValid() == false || n < 0) return;

	LBListView *owner = cast_as(ScopeHandler(), LBListView);

	uint8 fontsize = owner->FittingFontSize(r);
	if(fontsize == 0) return;
	owner->SetFontSize(fontsize);

	uint16 w = owner->StringWidth(str);
	bool erase_mode = false;
	if(owner->BorderStyle() == LBK_LIST_VIEW_INTERLACED_ROWS && (n & 0x01)) erase_mode = true;
 
	switch(owner->ItemsAlignment())
	{
		case B_ALIGN_LEFT:
			owner->DrawString(str,
					  BPoint(r.left + 2, r.Center().y - fontsize / 2.f),
					  erase_mode);
			break;

		case B_ALIGN_CENTER:
			owner->DrawString(str,
					  r.Center() - BPoint(w / 2.f, fontsize / 2.f),
					  erase_mode);
			break;

		default: // B_ALIGN_RIGHT
			owner->DrawString(str,
					  BPoint(r.right - w - 2.f, r.Center().y - fontsize / 2.f),
					  erase_mode);
			break;
	}
}


LBListStringItem::LBListStringItem(const char *text)
	: LBListItem(),
	  fText(NULL)
{
	SetText(text);
}


LBListStringItem::~LBListStringItem()
{
	if(fText != NULL) free(fText);
}


const char*
LBListStringItem::Text() const
{
	return fText;
}


void
LBListStringItem::SetText(const char *text)
{
	if(fText != NULL) free(fText);
	fText = (text != NULL ? strdup(text) : NULL);

	if(IsVisible() == false) return;

	LBListView *view = (ScopeHandler() == NULL ? NULL : cast_as(ScopeHandler(), LBListView));
	if(view == NULL || view->IsActivated() == false) return;

	view->InvalidateItem(view->IndexOf(this));
}


void
LBListStringItem::Draw(BRect r, int32 n)
{
	LBListView *owner = (ScopeHandler() == NULL ? NULL : cast_as(ScopeHandler(), LBListView));
	if(owner == NULL || owner->IsActivated() == false || fText == NULL) return;

	if(owner->IsSelectable())
	{
		r.left += r.Height() + 1.f;
		r.right -= r.Height() + 1.f;
	}

	DrawString(fText, r, n);
}


LBListControlItem::LBListControlItem(const char *label, int32 value)
	: LBListItem(),
	  fLabel(NULL),
	  fValue(value)
{
	SetLabel(label);
}


LBListControlItem::~LBListControlItem()
{
	if(fLabel != NULL) free(fLabel);
}


const char*
LBListControlItem::Label() const
{
	return fLabel;
}


void
LBListControlItem::SetLabel(const char *label)
{
	if(fLabel != NULL) free(fLabel);
	fLabel = (label != NULL ? strdup(label) : NULL);

	if(IsVisible() == false) return;

	LBListView *view = (ScopeHandler() == NULL ? NULL : cast_as(ScopeHandler(), LBListView));
	if(view == NULL || view->IsActivated() == false) return;

	view->InvalidateItem(view->IndexOf(this));
}


int32
LBListControlItem::Value() const
{
	return fValue;
}


void
LBListControlItem::SetValue(int32 value)
{
	if(fValue == value) return;
	fValue = value;

	if(IsVisible() == false) return;

	LBListView *view = (ScopeHandler() == NULL ? NULL : cast_as(ScopeHandler(), LBListView));
	if(view == NULL || view->IsActivated() == false) return;

	view->InvalidateItem(view->IndexOf(this));
}


void
LBListControlItem::Draw(BRect rect, int32 n)
{
	LBListView *owner = (ScopeHandler() == NULL ? NULL : cast_as(ScopeHandler(), LBListView));
	if(owner == NULL || owner->IsActivated() == false) return;

	bool erase_mode = false;
	if(owner->BorderStyle() == LBK_LIST_VIEW_INTERLACED_ROWS && (n & 0x01)) erase_mode = true;
 
	if(owner->IsSelectable())
	{
		rect.left += rect.Height() + 1.f;
		rect.right -= rect.Height() + 1.f;
	}

	BRect r = rect;
	r.right -= 3.f * r.Height() + 1.f;
	if(r.IsValid() == false) return;
	DrawString(fLabel, r, n);

	r.left = r.right + 1.f;
	r.right = rect.right;
	r.InsetBy(2, 3);
	if(r.IsValid() == false) return;
	owner->StrokeRect(r, erase_mode);

	if(fValue == B_CONTROL_OFF)
		r.right = r.left + r.Width() / 3.f;
	else
		r.left = r.right - r.Width() / 3.f;
	r.InsetBy(2, -2);
	owner->FillRect(r, erase_mode ? B_SOLID_LOW : B_SOLID_HIGH);
	if(fValue == B_CONTROL_ON)
		owner->StrokeRect(r.InsetBySelf(2, 2), !erase_mode);
}

