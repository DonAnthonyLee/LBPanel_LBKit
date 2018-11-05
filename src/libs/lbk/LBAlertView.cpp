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
 * File: LBAlertView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKConfig.h>
#include <lbk/LBAlertView.h>

// Comment the line below out when it's SURE to use it
#if LBK_KEY_TYPICAL_NUMBER < 2
#error "LBAlertView: Usually, it's useless when number of keys less than 2 !!!"
#endif

#define ICON_IS_16x16(id)	((id) > LBK_ICON_ID_16x16_BEGIN && (id) < LBK_ICON_ID_16x16_END)
#define ICON_IS_VALID(id)	((id) == LBK_ICON_NONE || ICON_IS_16x16(id))


LBAlertView::LBAlertView(const char *title,
			 const char *text,
			 lbk_icon_id icon3,
			 lbk_icon_id icon2,
			 lbk_icon_id icon1,
			 alert_type type)
	: LBView(NULL),
	  fTitle(title),
	  fText(text),
	  fButtonMask(0),
	  fInvoker(NULL)
{
	// lbk_icon_id is enum type (int), initialize each one of array instead of "memset".
	for(size_t k = 0; k < sizeof(fIcons) / sizeof(fIcons[0]); k++)
		fIcons[k] = LBK_ICON_NONE;

	switch(type)
	{
		case B_IDEA_ALERT:
			fIcons[3] = LBK_ICON_IDEA;
			break;

		case B_INFO_ALERT:
			fIcons[3] = LBK_ICON_INFO;
			break;

		case B_STOP_ALERT:
			fIcons[3] = LBK_ICON_STOP;
			break;

		case B_WARNING_ALERT:
			fIcons[3] = LBK_ICON_WARNING;
			break;

		default:
			break;
	}

	SetButtonIcon(0, icon1);
	SetButtonIcon(1, icon2);
	SetButtonIcon(2, icon3);
	SetButtonAlignment(B_ALIGN_RIGHT);
}


LBAlertView::~LBAlertView()
{
	if(fInvoker != NULL) delete fInvoker;
}


void
LBAlertView::SetTitle(const char *title)
{
	fTitle.Truncate(0);
	fTitle.SetTo(title);

	BRect r = LBView::Bounds();
	r.bottom = 13;
	Invalidate(r);
}


void
LBAlertView::SetText(const char *text)
{
	fText.Truncate(0);
	fText.SetTo(text);

	BRect r = LBView::Bounds();
	r.left = (fIcons[3] == LBK_ICON_NONE ? 13 : 45);
	r.top = 14;
	r.bottom -= 18;
	r.InsetBy(0, 2);
	Invalidate();
}


void
LBAlertView::SetButtonIcon(int32 index, lbk_icon_id idIcon)
{
	if(index < 0 || index > min_c(2, LBK_KEY_TYPICAL_NUMBER - 1)) return;
	if(!ICON_IS_VALID(idIcon)) return;

	if(fIcons[index] != idIcon)
	{
		fIcons[index] = idIcon;

		BRect r = LBView::Bounds();
		r.top = r.bottom - 17;
		Invalidate(r);
	}
}


status_t
LBAlertView::SetInvoker(BInvoker *invoker)
{
	if(invoker == fInvoker) return B_BAD_VALUE;

	if(fInvoker != NULL)
		delete fInvoker;
	fInvoker = invoker;

	return B_OK;
}


void
LBAlertView::SetButtonAlignment(alignment align)
{
	uint8 mask = 0xff;
	mask <<= LBK_KEY_TYPICAL_NUMBER;
	mask = ~mask;

	switch(align)
	{
		case B_ALIGN_LEFT:
			mask &= 0x07;
			break;

		case B_ALIGN_CENTER:
			mask &= (0x07 << max_c(0, (LBK_KEY_TYPICAL_NUMBER - 3) / 2));
			break;

		default:
			mask &= (0x07 << max_c(0, LBK_KEY_TYPICAL_NUMBER - 3));
	}

	if(fButtonMask != mask)
	{
		fButtonMask = mask;

		BRect r = LBView::Bounds();
		r.top = r.bottom - 17;
		Invalidate(r);
	}
}


void
LBAlertView::DrawButtonIcon(lbk_icon_id idIcon, BPoint location)
{
	// for LBK_KEY_TYPICAL_NUMBER <= 2, so on
	DrawIcon(idIcon, location);
}


void
LBAlertView::Draw(BRect rect)
{
	uint16 w;
	BRect r;

	// title
	r = LBView::Bounds();
	r.bottom = 13;
	if(fTitle.Length() > 0 && r.Intersects(rect))
	{
		SetFontSize(12);
		w = StringWidth(fTitle);
		DrawString(fTitle.String(), BPoint(r.left + r.Width() / 2.f - w / 2.f, 1));
	}

	// message area
	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom - 18;
	if(r.Intersects(rect))
	{
		FillRect(r & rect);
	}

	// type icon
	if(fIcons[3] != LBK_ICON_NONE)
	{
		const lbk_icon *icon = lbk_get_icon_data(fIcons[3]);

		BRect tRect = r;
		tRect.right = 32;
		tRect.OffsetBy(10, 0);

		if(icon != NULL && tRect.Intersects(rect))
		{
			lbk_icon icon_inverse;
			icon_inverse.type = icon->type;
			for(size_t k = 0; k < sizeof(icon_inverse.data); k++)
				icon_inverse.data[k] = ~(icon->data[k]);
			DrawIcon(&icon_inverse, tRect.LeftTop());
		}
	}

	// text
	if(fText.Length() > 0)
	{
		BRect tRect = r;
		tRect.left = (fIcons[3] == LBK_ICON_NONE ? 13 : 45);
		tRect.InsetBy(0, 2);
		if(tRect.Intersects(rect))
		{
			BString tmpStr(fText);
			int32 found = tmpStr.FindFirst("\n");

			SetFontSize(12);
			if(found > 0)
				tmpStr.Truncate(found);
			DrawString(tmpStr.String(),
				   found > 0 ? tRect.LeftTop() : (tRect.LeftTop() + BPoint(0, 8)),
				   true);
			if(found > 0)
			{
				tmpStr.SetTo(fText.String() + found + 1);
				DrawString(tmpStr.String(), tRect.LeftTop() + BPoint(0, 16), true);
			}
		}
	}

	// icon
	r = LBView::Bounds();
	r.top = r.bottom - 16;
	r.bottom -= 1;
	r.right = r.Width() / (float)LBK_KEY_TYPICAL_NUMBER - 1.f;

	int32 idBtn = 0;
	for(int k = 0; k < LBK_KEY_TYPICAL_NUMBER && idBtn < 3; k++)
	{
		if((fButtonMask & (0x01 << k)) != 0)
		{
			if(ICON_IS_16x16(fIcons[idBtn]) && r.Intersects(rect))
			{
				BPoint pt = r.LeftTop() + BPoint(r.Width() / 2.f, r.Height() / 2.f) - BPoint(7, 7);
				uint8 pressed = 0;

				KeyState(&pressed);
				if(pressed & (0x01 << k)) pt += BPoint(1, 1);

				DrawButtonIcon(fIcons[idBtn], pt);
			}
			idBtn++;
		}
		r.OffsetBy(r.Width() + 1, 0);
	}
}


void
LBAlertView::KeyDown(uint8 key, uint8 clicks)
{
	if(clicks == 1 && (fButtonMask & (0x01 << key)) != 0)
	{
		BRect r = LBView::Bounds();
		r.top = r.bottom - 17;

		Invalidate(r);
	}
}


void
LBAlertView::KeyUp(uint8 key, uint8 clicks)
{
	BRect r = LBView::Bounds();
	r.top = r.bottom - 17;
	Invalidate(r);

	if((fButtonMask & (0x01 << key)) == 0) return;

	int32 idBtn = 0;
	for(uint8 k = 0; k < LBK_KEY_TYPICAL_NUMBER && idBtn < 3; k++)
	{
		if((fButtonMask & (0x01 << k)) == 0) continue;
		if(k == key) break;
		idBtn++;
	}
	if(!ICON_IS_16x16(fIcons[idBtn])) return;

	if(fInvoker != NULL)
	{
		BMessage aMsg = *(fInvoker->Message());
		aMsg.AddInt32("which", idBtn);
		aMsg.AddInt8("clicks", *((int8*)&clicks));
		fInvoker->Invoke(&aMsg);
	}
}

