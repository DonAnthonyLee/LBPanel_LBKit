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
 * File: VPDApp.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_VIRTUAL_PANEL_DEVICE_APP_H__
#define __LBK_VIRTUAL_PANEL_DEVICE_APP_H__

#include <be/Be.h>

#include <lbk/add-ons/LBPanelBuffer.h>

#ifdef __cplusplus /* Just for C++ */

class VPDApplication : public BApplication {
public:
	VPDApplication();
	virtual ~VPDApplication();
};


class VPDWindow : public BWindow {
public:
	VPDWindow(BRect frame,
		  const char* title,
		  uint16 w, uint16 h,
		  uint8 keys_count,
		  uint8 point_size,
		  int32 id,
		  lbk_color_space cspace);
	virtual ~VPDWindow();

	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);
};

#endif /* __cplusplus */

#endif /* __LBK_VIRTUAL_PANEL_DEVICE_APP_H__ */

