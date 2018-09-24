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
 * File: OLEDMenuItem.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_MENU_ITEM_H__
#define __OLED_MENU_ITEM_H__

#include <be/Be.h>
#include <OLEDIconDefs.h>

#ifdef __cplusplus /* Just for C++ */

class OLEDMenuView;

class OLEDMenuItem : public EInvoker {
public:
	OLEDMenuItem(const char *label,
		     BMessage *message,
		     oled_icon_id idIcon);
	virtual ~OLEDMenuItem();

	const char*	Label() const;
	void		SetLabel(const char *label);

	oled_icon_id	Icon() const;
	void		SetIcon(oled_icon_id idIcon);

	void		Show();
	void		Hide();
	bool		IsHidden() const;

private:
	friend OLEDMenuView;

	BString fLabel;
	oled_icon_id fIcon;
	bool fHidden;

	OLEDMenuView *fMenuView;
};

#endif /* __cplusplus */

#endif /* __OLED_MENU_ITEM_H__ */

