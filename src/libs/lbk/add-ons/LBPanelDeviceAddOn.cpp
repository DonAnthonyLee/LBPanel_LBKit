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
 * File: LBPanelDeviceAddOn.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelDeviceAddOn.h>
#include <lbk/add-ons/LBPanelDevice.h>

// use ETK++'s functions for no implementation in Lite BeAPI
#ifdef ETK_MAJOR_VERSION
	#define B_SYMBOL_TYPE_TEXT				0x02

	typedef void*	image_id;
	#define load_add_on(path)				etk_load_addon(path)
	#define unload_add_on(image)				etk_unload_addon(image)
	#define get_image_symbol(image, name, sclass, ptr)	etk_get_image_symbol(image, name, ptr)
	#define IMAGE_IS_VALID(image)				(image != NULL)
#else
	#define IMAGE_IS_VALID(image)				(image > 0)
#endif


LBPanelDeviceAddOn::LBPanelDeviceAddOn()
	: fID(-1), fDev(NULL), fAddOn(NULL)
{
	// TODO
}


LBPanelDeviceAddOn::~LBPanelDeviceAddOn()
{
	// TODO
}


status_t
LBPanelDeviceAddOn::SendMessage(const BMessage *msg)
{
	if(msg == NULL || fID < 0 || fDev == NULL) return B_BAD_VALUE;

	if(fDev == NULL || cast_as(fDev, LBPanelDeviceAddOn) == this)
		return fMsgr.SendMessage(msg);
	else
		return fDev->SendMessage(msg);
}


status_t
LBPanelDeviceAddOn::SendMessage(uint32 command)
{
	BMessage msg(command);
	return SendMessage(&msg);
}


int32
LBPanelDeviceAddOn::Index() const
{
	return fID;
}


LBPanelDevice*
LBPanelDeviceAddOn::Panel() const
{
	return((fID < 0) ? NULL : fDev);
}


void*
LBPanelDeviceAddOn::LoadAddOn(const char *add_on,
			      void **ptr,
			      const char *name) const
{
	void *retVal;
	image_id image;
	BPath pth(add_on, NULL, true);

	if(pth.Path() == NULL || ptr == NULL || name == NULL) return NULL;

	image = load_add_on(pth.Path());
	if(!IMAGE_IS_VALID(image)) return NULL;

	if(get_image_symbol(image, name, B_SYMBOL_TYPE_TEXT, ptr) != B_OK)
	{
		unload_add_on(image);
		return NULL;
	}

#ifdef ETK_MAJOR_VERSION
	retVal = image;
#else
	retVal = reinterpret_cast<void*>((long)image);
#endif

	return retVal;
}


status_t
LBPanelDeviceAddOn::UnloadAddOn(void *m) const
{
	image_id image;

#ifdef ETK_MAJOR_VERSION
	image = m;
#else
	image = (image_id)reinterpret_cast<long>(m);
#endif

	return(IMAGE_IS_VALID(image) ? unload_add_on(image) : B_BAD_VALUE);
}

