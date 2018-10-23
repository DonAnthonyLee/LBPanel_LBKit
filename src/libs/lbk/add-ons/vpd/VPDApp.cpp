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
 * File: VPDApp.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include "VPDApp.h"
#include "VPDView.h"

#define VPD_MSG_CAPTRUE		'vmgc'
#define VPD_MSG_KEY		'vmgk'


VPDApplication::VPDApplication()
	: BApplication("application/x-vnd.lbk-vpd-app")
{
}


VPDApplication::~VPDApplication()
{
}


VPDWindow::VPDWindow(BRect frame, const char* title, uint16 w, uint16 h, uint8 keys_count, int32 id)
	: BWindow(frame, title,
		  (title == NULL || *title == 0) ? B_BORDERED_WINDOW : B_TITLED_WINDOW,
		  B_NOT_RESIZABLE | B_QUIT_ON_WINDOW_CLOSE)
{
	BRect r;
	frame.OffsetTo(0, 0);

	if(Type() == B_TITLED_WINDOW)
	{
		r = frame;
		r.bottom = r.top + 16;

		BMenuBar *menubar = new BMenuBar(r, NULL);

		BMenu *menu = new BMenu("File", B_ITEMS_IN_COLUMN);
		menu->AddItem(new BMenuItem("Capture screen...", new BMessage(VPD_MSG_CAPTRUE)));
		menu->AddSeparatorItem();
		menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'q', B_COMMAND_KEY));
		menubar->AddItem(menu);

		AddChild(menubar);
		menubar->ResizeToPreferred();

		BMenuItem *item;
		for(int32 k = 0; (item = menu->ItemAt(k)) != NULL; k++) item->SetTarget(BMessenger(this));

		frame.top = menubar->Frame().bottom + 1;
	}

	r = frame;
	if(r.Width() > (float)(w - 1))
		r.InsetBy((r.Width() - (float)(w - 1)) / 2.f, 0);
	r.bottom = r.top + (float)(h - 1);
	AddChild(new VPDView(r, "screen", B_FOLLOW_NONE));
	frame.top = r.bottom + 1;

	if(keys_count == 0) return;

	r = frame;
	r.right = r.left + r.Width() / (float)keys_count - 1.f;

	for(uint8 m = 0; m < keys_count; m++)
	{
		BString label;
		label << "K" << m + 1;

		BMessage *msg = new BMessage(VPD_MSG_KEY);
		msg->AddInt8("id", (int8)m);

		BButton *btn = new BButton(r.InsetByCopy(5, 2), NULL, label.String(), msg, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
		AddChild(btn);

		r.OffsetBy(r.Width() + 1, 0);
	}
}


VPDWindow::~VPDWindow()
{
}


bool
VPDWindow::QuitRequested()
{
	// TODO
	return false;
}


void
VPDWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case VPD_MSG_KEY:
			// TODO
			break;

		case VPD_MSG_CAPTRUE:
			// TODO
			break;

		default:
			BWindow::MessageReceived(msg);
	}
}

