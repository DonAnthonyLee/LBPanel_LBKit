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
 * File: OLEDMenuItem.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <string.h>
#include <OLEDMenuItem.h>
#include <OLEDMenuView.h>

#define ICON_IS_32x32(id)	((id) > OLED_ICON_ID_32x32_BEGIN && (id) < OLED_ICON_ID_32x32_END)


OLEDMenuItem::OLEDMenuItem(const char *label,
			   BMessage *message,
			   oled_icon_id idIcon)
	: BInvoker(message, NULL),
	  fLabel(NULL),
	  fIcon(OLED_ICON_NONE),
	  fHidden(false),
	  fMenuView(NULL)
{
	SetLabel(label);
	SetIcon(idIcon);
}


OLEDMenuItem::~OLEDMenuItem()
{
	/*
	 * WARNING: NO GUARANTEE for MenuView !!!
	 * 	item should be removed from MenuView before deleting
	 */
	if(fLabel != NULL) free(fLabel);
}


const char*
OLEDMenuItem::Label() const
{
	return fLabel;
}


void
OLEDMenuItem::SetLabel(const char *label)
{
	if(fLabel != NULL) free(fLabel);
	fLabel = (label != NULL ? strdup(label) : NULL);

	// NO REDRAW
}


oled_icon_id
OLEDMenuItem::Icon() const
{
	return fIcon;
}


void
OLEDMenuItem::SetIcon(oled_icon_id idIcon)
{
	// only 32x32
	fIcon = (ICON_IS_32x32(idIcon) ? idIcon : OLED_ICON_NONE);

	// NO REDRAW
}


bool
OLEDMenuItem::IsHidden() const
{
	return fHidden;
}

