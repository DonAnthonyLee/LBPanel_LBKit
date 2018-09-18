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
 * File: OLEDPageView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_PAGE_VIEW_H__
#define __OLED_PAGE_VIEW_H__

#include "OLEDView.h"

#ifdef __cplusplus /* Just for C++ */

class OLEDPageView : public OLEDView {
public:
	OLEDPageView(uint8 num_of_nav_buttons, const char *name = NULL);
	virtual ~OLEDPageView();

	virtual void	ShowNavButton(uint8 idBtn);
	virtual void	HideNavButton(uint8 idBtn);
	bool		IsNavButtonHidden(uint8 idBtn) const;

	void		SetNavButtonIcon(int32 idBtn, oled_icon_id idIcon);
	oled_icon_id	GetNavButtonIcon(int32 idBtn) const;
	virtual void	DrawNavButton(oled_icon_id idIcon, BPoint location);

	virtual BRect	Bounds() const;

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);

	virtual void	Activated(bool state);

	// TODO

private:
	uint8 fNavButtonsState;
	oled_icon_id fButtonIcons[8]; // enough
};

#endif /* __cplusplus */

#endif /* __OLED_PAGE_VIEW_H__ */

