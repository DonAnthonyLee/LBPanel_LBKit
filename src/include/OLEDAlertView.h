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
 * File: OLEDAlertView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_ALERT_VIEW_H__
#define __OLED_ALERT_VIEW_H__

#include <OLEDView.h>

#ifdef __cplusplus /* Just for C++ */

class OLEDAlertView : public OLEDView {
public:
	OLEDAlertView(const char *title,
		      const char *text,
		      oled_icon_id button3_icon,
		      oled_icon_id button2_icon = OLED_ICON_NONE,
		      oled_icon_id button1_icon = OLED_ICON_NONE,
		      alert_type type = B_INFO_ALERT);
	virtual ~OLEDAlertView();

	int32		Go();
	status_t	Go(BInvoker *invoker);

	void		SetTitle(const char *title);
	void		SetText(const char *text);
	void		SetButtonIcon(int32 index, oled_icon_id icon);

private:
	BString fTitle;
	BString fText;
	oled_icon_id fIcons[4];
};

#endif /* __cplusplus */

#endif /* __OLED_ALERT_VIEW_H__ */

