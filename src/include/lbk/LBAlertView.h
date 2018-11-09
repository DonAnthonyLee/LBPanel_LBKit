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
 * File: LBAlertView.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_ALERT_VIEW_H__
#define __LBK_ALERT_VIEW_H__

#include <lbk/LBView.h>

#if defined(ETK_MAJOR_VERSION) && defined(ETK_LITE_BUILD)
enum alert_type {
	B_EMPTY_ALERT = 0,
	B_INFO_ALERT,
	B_IDEA_ALERT,
	B_WARNING_ALERT,
	B_STOP_ALERT
};
#endif

#ifdef __cplusplus /* Just for C++ */

class _EXPORT LBAlertView : public LBView {
public:
	LBAlertView(const char *title,
		    const char *text,
		    lbk_icon_id button3_icon,
		    lbk_icon_id button2_icon = LBK_ICON_NONE,
		    lbk_icon_id button1_icon = LBK_ICON_NONE,
		    alert_type type = B_INFO_ALERT);
	virtual ~LBAlertView();

	/*
	 * SetInvoker():
	 * 	the "invoker" will be deleted automatically when new invoker set
	 */
	status_t	SetInvoker(BInvoker *invoker);

	void		SetTitle(const char *title);
	void		SetText(const char *text);
	void		SetButtonIcon(int32 index, lbk_icon_id idIcon);
	void		SetButtonAlignment(alignment align);

	virtual void	Draw(BRect updateRect);
	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);

protected:
	virtual void	DrawButtonIcon(lbk_icon_id idIcon, BPoint location);
	int32		IndexOfButton(uint8 key) const;
	lbk_icon_id	GetButtonIcon(int32 index) const;

private:
	BString fTitle;
	BString fText;
	lbk_icon_id fIcons[4];
	uint8 fButtonMask;
	BInvoker *fInvoker;
};

#endif /* __cplusplus */

#endif /* __LBK_ALERT_VIEW_H__ */

