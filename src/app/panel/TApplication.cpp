/* --------------------------------------------------------------------------
 *
 * Panel application for little board
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
 * File: TApplication.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "TApplication.h"


TApplication::TApplication(const LBAppSettings *settings)
	: LBApplication(settings),
	  fScreenOffTimeout(0)
{
	// TODO: get custom menu for "settings"
}


TApplication::~TApplication()
{
	EmptyCustomMenu();
}


void
TApplication::EmptyCustomMenu()
{
	for(int32 k = 0; k < fCustomMenu.CountItems(); k++)
	{
		t_menu_item *item = (t_menu_item*)fCustomMenu.ItemAt(k);

		if(item->title) free(item->title);
		if(item->command) free(item->command);
		if(item->args) free(item->args);
		free(item);
	}
	fCustomMenu.MakeEmpty();
}


int32
TApplication::CountCustomMenuItems() const
{
	return fCustomMenu.CountItems();
}


const t_menu_item*
TApplication::CustomMenuItemAt(int32 index) const
{
	t_menu_item *item = (t_menu_item*)fCustomMenu.ItemAt(index);
	return item;
}


void
TApplication::Pulse()
{
	// TODO
}


void
TApplication::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case LBK_APP_SETTINGS_UPDATED:
			// TODO: update custom menu, screen off timeout, etc.
			break;

		default:
			LBApplication::MessageReceived(msg);
	}
};

