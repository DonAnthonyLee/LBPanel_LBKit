/* --------------------------------------------------------------------------
 *
 * Little Board Application Kit
 * Copyright (C) 2018-2019, Anthony Lee, All Rights Reserved
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
 * File: LBPanelDeviceAddOn.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_PANEL_DEVICE_ADD_ON_H__
#define __LBK_PANEL_DEVICE_ADD_ON_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus /* Just for C++ */

class LBApplication;
class LBPanelDevice;
class LBPanelCombiner;

class _EXPORT LBPanelDeviceAddOn {
public:
	LBPanelDeviceAddOn();
	virtual ~LBPanelDeviceAddOn();

	virtual status_t	SendMessage(const BMessage *msg);
	status_t		SendMessage(uint32 command);

	int32			Index() const;
	LBPanelDevice*		Panel() const;

	static void*		LoadAddOn(const char *add_on,
					  void **ptr,
					  const char *name);
	static status_t		UnloadAddOn(void *image);

private:
	friend class LBPanelApplication;
	friend class LBPanelDevice;
	friend class LBPanelCombiner;

	int32 fID;
	BMessenger fMsgr;

	LBPanelDevice *fDev;
	void *fAddOn;
};

#endif /* __cplusplus */

#endif /* __LBK_PANEL_DEVICE_ADD_ON_H__ */

