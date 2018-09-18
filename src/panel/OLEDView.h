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
 * File: OLEDView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_VIEW_H__
#define __OLED_VIEW_H__

#include <be/Be.h>
#include <OLEDIconDefs.h>

#ifdef __cplusplus /* Just for C++ */

class OLEDApp;

class OLEDView : public BLooper {
public:
	OLEDView();
	virtual ~OLEDView();

	// Bounds(): A derived class should draw only in this region.
	virtual BRect	Bounds() const;

	void		FillRect(BRect r, pattern p = B_SOLID_HIGH);
	void		StrokeRect(BRect r, bool erase = false);
	void		DrawString(const char *string, BPoint location, bool erase = false);
	void		DrawIcon(const oled_icon *icon, BPoint location);

	virtual void	DrawIcon(oled_icon_id idIcon, BPoint location);

	uint8		FontSize() const;
	void		SetFontSize(uint8 size);
	uint16		StringWidth(const char *string) const;

	void		EnableUpdate(bool state);
	bool		IsNeededToRegen() const;

	bool		GetPowerState() const;
	void		SetPowerState(bool state);

	// Empty functions BEGIN --- just for derivative class
	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	// Empty functions END

	uint8		KeyState(bool *pressed = NULL) const;

	bool		IsActivated() const;
	virtual void	Activated(bool state);

	virtual void	MessageReceived(BMessage *msg);

private:
	friend class OLEDApp;

	int fFD;
	uint64 fTimestamp;
	bool fActivated;

	uint8 fFontSize;
	uint16 fKeyState;

	void		SetActivated(bool state);
};

#endif /* __cplusplus */

#endif /* __OLED_VIEW_H__ */

