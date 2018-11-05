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
 * File: VPDView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "VPDApp.h"
#include "VPDView.h"
#include "font8.h"

#ifdef ETK_MAJOR_VERSION
	#ifdef ETK_BIG_ENDIAN
		#define B_HOST_IS_BENDIAN	1
	#else
		#define B_HOST_IS_BENDIAN	0
	#endif
#endif


VPDView::VPDView(BRect frame, const char* title, uint32 resizingMode)
	: BView(frame, title, resizingMode, B_WILL_DRAW),
	  fPointSize(1),
	  fLabel(NULL),
	  fPowerState(true),
	  fBufferUpdateEnabled(true)
{
	SetViewColor(230, 230, 230);
	SetLowColor(230, 230, 230);
}


VPDView::~VPDView()
{
	if(fLabel != NULL) free(fLabel);
}


void
VPDView::SetPointSize(uint8 s)
{
	if(fPointSize != s && s > 0)
	{
		fPointSize = s;
		Invalidate();
	}
}


void
VPDView::SetLabel(const char *str)
{
	if(fLabel != NULL)
	{
		free(fLabel);
		fLabel = NULL;
	}

	if(!(str == NULL || *str == 0))
		fLabel = strdup(str);

	Invalidate();
}


bool
VPDView::SetFontHeight(uint8 h)
{
	if(h < 8 || h > 48) return false;
	if(h == 8) return true;

	BFont font;
	font_height fontHeight;

	GetFont(&font);

	uint8 t = h - ((h < 24) ? 4 : 8);
	for(uint8 k = 0; k < 8; k++)
	{
		font.SetSize(t + k);
		font.GetHeight(&fontHeight);

		if(fontHeight.ascent + fontHeight.descent >= (float)h ||
		   (float)h - (fontHeight.ascent + fontHeight.descent) < 1.1f)
		{
			SetFont(&font, B_FONT_SIZE);
			return true;
		}
	}

	return false;
}


void
VPDView::SetPowerState(bool state)
{
	if(fPowerState != state)
	{
		fPowerState = state;
		Invalidate();
	}
}


uint8*
VPDView::Buffer() const
{
	return (uint8*)fBuffer.Bits();
}


size_t
VPDView::BufferLength() const
{
	return fBuffer.BitsLength();
}


void
VPDView::ResizeBuffer(uint16 w, uint16 h, lbk_color_space cspace)
{
	if(fBuffer.ResizeTo(w, h, cspace)) Invalidate();
}


BRect
VPDView::DrawingArea(uint16 x, uint16 y, uint16 w, uint16 h) const
{
	BRect r;

	if(x < fBuffer.Width() && y < fBuffer.Height())
	{
		if(w > fBuffer.Width() - x)
			w = fBuffer.Width() - x;
		if(h > fBuffer.Height() - y)
			h = fBuffer.Height() - y;

		if(w > 0 && h > 0)
		{
			uint32 space = (fPointSize < 2) ? 0 : ((fPointSize < 4) ? 1 : 2);

			r.left = (float)(x * ((uint32)fPointSize + space));
			r.top = (float)(y * ((uint32)fPointSize + space));
			r.right = r.left + (float)((uint32)w * (uint32)fPointSize + (uint32)(w - 1) * space - 1);
			r.bottom = r.top + (float)((uint32)h * (uint32)fPointSize + (uint32)(h - 1) * space - 1);
		}
	}

	return r;
}


void
VPDView::GetPreferredSize(float *width, float *height)
{
	BRect r = DrawingArea(0, 0, fBuffer.Width(), fBuffer.Height());

	if(width != NULL)
		*width = r.IsValid() ? r.Width() : 0;

	if(height != NULL)
		*height = r.IsValid() ? r.Height() : 0;
}


void
VPDView::Draw(BRect updateRect)
{
	BRect r;
	uint16 space = (fPointSize < 2) ? 0 : ((fPointSize < 4) ? 1 : 2);

#if 1
	if(fBuffer.Depth() != 1) return;
#endif

	r.Set(0, 0, fPointSize - 1, fPointSize - 1);
	for(uint16 y = 0; y < fBuffer.Height(); y++)
	{
		for(uint16 x = 0; x < fBuffer.Width(); x++)
		{
			if(r.Intersects(updateRect))
			{
				rgb_color c = fBuffer.GetPixel(x, y);
				if(c.alpha == 0)
				{
					c = ViewColor();
				}
				else switch(fBuffer.Depth())
				{
					case 1:
						if(c == fBuffer.LowColor())
							c = ViewColor();
						else
							c.red = c.green = c.blue = (fPowerState ? ((fPointSize == 1) ? 50 : 0) : 200);
						break;

					default:
						// TODO
						break;
				}
				SetHighColor(c);
				FillRect(r, B_SOLID_HIGH);
			}
			r.OffsetBy(r.Width() + 1 + space, 0);
		}

		r.left = 0;
		r.right = fPointSize - 1;
		r.OffsetBy(0, r.Height() + 1 + space);
	}

	if(fLabel != NULL || fPowerState == false)
	{
		BString str(fPowerState ? fLabel : "Power Off");
		BFont font;

		PushState();

		GetFont(&font);
		font.SetSize(Bounds().Height() / (fPowerState ? 2 : 3));
		SetFont(&font, B_FONT_SIZE);

		font_height fontHeight;
		font.GetHeight(&fontHeight);

		float w = font.StringWidth(str.String());
		float h = fontHeight.ascent + fontHeight.descent;

		r = Bounds();
		r.InsetBy((Bounds().Width() - w) / 2.f, (Bounds().Height() - h) / 2.f);
		if(r.Intersects(updateRect))
		{
			SetDrawingMode(B_OP_ALPHA);
			SetHighColor(100, 100, 100, 150);
			MovePenTo(r.LeftTop() + BPoint(0, fontHeight.ascent));
			DrawString(str.String());
		}

		PopState();
	}
}


BRect
VPDView::FillRectOnBuffer(uint16 x, uint16 y, uint16 w, uint16 h, pattern p, bool patternVertical)
{
	fBuffer.FillRect(x, y, w, h, p, patternVertical);

	return DrawingArea(x, y, w, h);
}


BRect
VPDView::DrawStringOnBuffer(const char *str, uint16 x, uint16 y, bool erase_mode)
{
	BRect retVal;

	if(fBuffer.Bits() == NULL ||
	   str == NULL || *str == 0 ||
	   x >= fBuffer.Width() || y >= fBuffer.Height()) return retVal;

	BFont font;
	font_height fontHeight;

	GetFont(&font);
	font.GetHeight(&fontHeight);

	uint16 w = (uint16)font.StringWidth(str);
	uint16 h = (uint16)(fontHeight.ascent + fontHeight.descent);
	if(w > fBuffer.Width() - x)
		w = fBuffer.Width() - x;
	if(h > fBuffer.Height() - y)
		h = fBuffer.Height() - y;

	retVal = DrawingArea(x, y, w, h);
	if(retVal.IsValid() == false) return retVal;

	BBitmap *bitmap = new BBitmap(BRect(0, 0, w - 1, h - 1), B_RGB32, true);
	BView *view = new BView(bitmap->Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);

	view->SetFont(&font, B_FONT_ALL);
	if(fBuffer.Depth() == 1)
		view->ForceFontAliasing(false);
	view->SetHighColor(0, 0, 0);
	view->SetLowColor(255, 255, 255);

	bitmap->Lock();
	bitmap->AddChild(view);
	view->FillRect(view->Bounds(), B_SOLID_LOW);
	view->MovePenTo(0, fontHeight.ascent);
	view->DrawString(str);
	view->Flush();
	view->Sync();
	bitmap->Unlock();

	const uint32 *data;
#ifdef ETK_MAJOR_VERSION
	EPixmap *pixmap = new EPixmap(bitmap->Bounds(), E_RGB32);
	bitmap->Lock();
	bitmap->GetPixmap(pixmap, bitmap->Bounds());
	bitmap->Unlock();
	data = (const uint32*)pixmap->Bits();
#else
	bitmap->LockBits();
	data = (const uint32*)bitmap->Bits();
#endif

	if(data != NULL)
	{
		for(uint16 yy = 0; yy < h; yy++)
		{
			const uint32 *bits = (const uint32*)(data + yy * w);

			for(uint16 xx = 0; xx < w; xx++)
			{
				rgb_color c;
				uint32 v = *bits++;
#if B_HOST_IS_BENDIAN
				c.red = (v >> 8) & 0xff;
				c.green = (v >> 16) & 0xff;
				c.blue = (v >> 24) & 0xff;
#else
				c.red = (v >> 16) & 0xff;
				c.green = (v >> 8) & 0xff;
				c.blue = v & 0xff;
#endif

				switch(fBuffer.Depth())
				{
					case 1:
						if(c.red > 150 && c.green > 150 && c.blue > 150) break;
						fBuffer.FillRect(x + xx, y + yy, 1, 1, erase_mode ? B_SOLID_LOW : B_SOLID_HIGH, true);
						break;

					default:
						// TODO
						break;
				}
			}
		}
	}

#ifdef ETK_MAJOR_VERSION
	delete pixmap;
#else
	bitmap->UnlockBits();
#endif
	delete bitmap;

	return retVal;
}


#ifdef ETK_MAJOR_VERSION
	#define FindData		BFindData
	#define B_POINTER_TYPE		E_POINTER_TYPE
#endif

void
VPDView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case VPD_MSG_POWER_STATE:
			{
				bool state;

				if(msg->FindBool("state", &state) != B_OK) break;
				SetPowerState(state);
			}
			break;

		case VPD_MSG_ENABLE_UPDATE:
			{
				bool update;

				if(msg->FindBool("update", &update) != B_OK) break;
				if(fBufferUpdateEnabled == update) break;

				fBufferUpdateEnabled = update;
				if(fBufferUpdateEnabled && fBufferUpdateRect.IsValid())
				{
					Invalidate(fBufferUpdateRect);
					fBufferUpdateRect = BRect();
				}
			}
			break;

		case VPD_MSG_FILL_RECT:
		case VPD_MSG_INVERT_RECT:
			{
				BRect r;
				pattern p;
				bool vpat;

				if(msg->FindRect("rect", &r) != B_OK) break;
				if(msg->what == VPD_MSG_FILL_RECT)
				{
					if(msg->FindInt64("pattern", (int64*)&p) != B_OK) break;
					if(msg->FindBool("vpattern", &vpat) != B_OK) break;
				}

				r = r & BRect(0, 0, fBuffer.Width() - 1, fBuffer.Height() - 1);
				if(r.IsValid() == false) break;

				if(msg->what == VPD_MSG_FILL_RECT)
					fBuffer.FillRect(r.left, r.top, r.Width() + 1, r.Height() + 1, p, vpat);
				else
					fBuffer.InvertRect(r.left, r.top, r.Width() + 1, r.Height() + 1);

				r = DrawingArea(r.left, r.top, r.Width() + 1, r.Height() + 1);
				if(r.IsValid() == false) break;

				if(fBufferUpdateEnabled)
					Invalidate(r);
				else if(fBufferUpdateRect.IsValid())
					fBufferUpdateRect = fBufferUpdateRect | r;
				else
					fBufferUpdateRect = r;
			}
			break;

		case VPD_MSG_DRAW_STRING:
			{
				BString str;
				BPoint pt;
				uint8 fontHeight;
				bool erase;

				if(msg->FindString("string", &str) != B_OK) break;
				if(msg->FindPoint("location", &pt) != B_OK) break;
				if(msg->FindInt8("height", (int8*)&fontHeight) != B_OK) break;
				if(msg->FindBool("erase_mode", &erase) != B_OK) break;

				if(str.Length() == 0) break;
				if(BRect(0, 0, fBuffer.Width() - 1, fBuffer.Height() - 1).Contains(pt) == false) break;
				if(SetFontHeight(fontHeight) == false) break;

				BRect r = (fontHeight == 8) ?
						DrawStringOnBuffer_h8(str.String(), pt.x, pt.y, erase) :
						DrawStringOnBuffer(str.String(), pt.x, pt.y, erase);
				if(r.IsValid() == false) break;

				if(fBufferUpdateEnabled)
					Invalidate(r);
				else if(fBufferUpdateRect.IsValid())
					fBufferUpdateRect = fBufferUpdateRect | r;
				else
					fBufferUpdateRect = r;
			}
			break;

		case VPD_MSG_STRING_WIDTH:
			{
				BString str;
				uint8 fontHeight;
				uint16 w = 0;

				if(msg->FindString("string", &str) != B_OK) break;
				if(msg->FindInt8("height", (int8*)&fontHeight) != B_OK) break;
				if(msg->IsSourceWaiting() == false) break;

				if(str.Length() > 0)
				{
					if(fontHeight == 8)
					{
						w = StringWidth_h8(str.String());
					}
					else
					{
						PushState();
						if(SetFontHeight(fontHeight))
						{
							BFont font;
							GetFont(&font);
							w = (uint16)font.StringWidth(str.String());
						}
						PopState();
					}
				}

				BMessage aMsg(B_REPLY);
				aMsg.AddInt16("width", *((int16*)&w));
				msg->SendReply(&aMsg);
			}
			break;

		case VPD_MSG_GET_BUFFER:
			{
				if(msg->IsSourceWaiting() == false) break;
				if(Buffer() == NULL)
				{
					msg->SendReply(B_NO_REPLY);
					break;
				}

				BMessage aMsg(B_REPLY);
				aMsg.AddData("buffer", B_POINTER_TYPE, fBuffer.Bits(), fBuffer.BitsLength(), true);
				msg->SendReply(&aMsg);
			}
			break;

		case VPD_MSG_SET_BUFFER:
			{
				const void *data = NULL;
				ssize_t nBytes = 0;

				if(msg->FindData("buffer", B_POINTER_TYPE, &data, &nBytes) != B_OK) break;
				if(data == NULL || nBytes != (ssize_t)fBuffer.BitsLength()) break;
				if(msg->IsSourceWaiting() == false) break;

				fBuffer.SetBits(data, 0, nBytes);
				msg->SendReply(B_REPLY);
			}
			break;

		case VPD_MSG_SYNC:
			if(msg->IsSourceWaiting())
				msg->SendReply(B_REPLY);
			break;

		default:
			BView::MessageReceived(msg);
	}
}

#ifdef ETK_MAJOR_VERSION
	#undef FindData
#endif


uint16
VPDView::StringWidth_h8(uint16 c) const
{
	uint16 w = 0;

	if(c == 0x65e0 || (c >= 0x0021 && c <= 0x007e))
		w = *((const uint8*)fontdata_h8 + (uint32)(((c >= 0x21 && c <= 0x7e) ? c : 0x7f) - 0x21) * 6);
	else if(c == 0x0020) // space
		w = 2;

	return w;
}


uint16
VPDView::StringWidth_h8(const char *str) const
{
	BString aStr(str);
	int32 count = aStr.CountChars();
	uint16 *ustr = NULL;
	uint16 w = 0;

	if(count > 0)
		ustr = (uint16*)malloc((count + 1) * 2);

	if(ustr != NULL)
	{
		bzero(ustr, (count + 1) * 2);

#ifdef ETK_MAJOR_VERSION
		aStr.CopyInto((eunichar *)ustr, (count + 1) * 2, 0, -1);
#else
		int32 state = 0;
		int32 srcLen = aStr.Length();
		int32 dstLen = count * 2;
		if(convert_from_utf8(B_UNICODE_CONVERSION,
				     aStr.String(), &srcLen,
				     (char*)ustr, &dstLen,
				     &state) != B_OK)
			count = 0;
#endif

		for(int32 k = 0; k < count; k++)
		{
			uint16 c = *(ustr + k);
			if(k > 0) w++;
			w += StringWidth_h8(c);
		}

		free(ustr);
	}

	return w;
}


BRect
VPDView::DrawStringOnBuffer_h8(const char *str, uint16 x, uint16 y, bool erase_mode)
{
	BString aStr(str);
	int32 count = aStr.CountChars();
	uint16 *ustr = NULL;

	if(fBuffer.Bits() == NULL ||
	   count == 0 ||
	   x >= fBuffer.Width() || y >= fBuffer.Height())
		return BRect();

	if(count > 0)
		ustr = (uint16*)malloc((count + 1) * 2);
	if(ustr == NULL)
		return BRect();

	bzero(ustr, (count + 1) * 2);

#ifdef ETK_MAJOR_VERSION
	aStr.CopyInto((eunichar *)ustr, (count + 1) * 2, 0, -1);
#else
	int32 state = 0;
	int32 srcLen = aStr.Length();
	int32 dstLen = count * 2;
	if(convert_from_utf8(B_UNICODE_CONVERSION,
			     aStr.String(), &srcLen,
			     (char*)ustr, &dstLen,
			     &state) != B_OK)
	{
		free(ustr);
		return BRect();
	}
#endif

	uint16 w = 0;
	for(int32 k = 0; k < count; k++)
	{
		uint16 c = *(ustr + k);
		uint16 w1 = StringWidth_h8(c);

		if(c == 0x65e0 || (c >= 0x21 && c <= 0x7e))
		{
			for(uint16 yy = 0; yy < 8 && (y + yy) < fBuffer.Height(); yy++)
			{
				const uint8 *bits = (const uint8*)fontdata_h8 + (uint32)(((c >= 0x21 && c <= 0x7e) ? c : 0x7f) - 0x21) * 6 + 1;

				for(uint16 xx = 0; xx < w1 && (x + w + xx) < fBuffer.Width(); xx++)
				{
					uint8 v = *bits++;
					v = ((v >> yy) & 0x01);

					switch(fBuffer.Depth())
					{
						case 1:
							if(v == 0) break;
							fBuffer.FillRect(x + w + xx, y + yy, 1, 1, erase_mode ? B_SOLID_LOW : B_SOLID_HIGH, true);
							break;

						default:
							// TODO
							break;
					}
				}
			}
		}

		w += w1;
		if(k != count - 1) w++;
	}

	free(ustr);

	uint16 h = 8;
	if(w > fBuffer.Width() - x)
		w = fBuffer.Width() - x;
	if(h > fBuffer.Height() - y)
		h = fBuffer.Height() - y;

	return DrawingArea(x, y, w, h);
}

