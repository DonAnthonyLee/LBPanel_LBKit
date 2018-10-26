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


VPDApplication::VPDApplication()
	: BApplication("application/x-vnd.lbk-vpd-app")
{
	// TODO
}


VPDApplication::~VPDApplication()
{
	// TODO
}


VPDWindow::VPDWindow(BRect frame, const char* title,
		     uint16 w, uint16 h,
		     uint8 keys_count,
		     uint8 point_size,
		     int32 id,
		     lbk_color_space cspace,
		     LBVPD *vpd)
	: BWindow(frame, title,
		  (title == NULL || *title == 0) ? B_BORDERED_WINDOW : B_TITLED_WINDOW,
		  B_NOT_RESIZABLE | B_QUIT_ON_WINDOW_CLOSE),
	  fVPD(vpd)
{
	BRect r;
	frame.OffsetTo(0, 0);

	if(Type() == B_TITLED_WINDOW)
	{
		r = frame;
		r.bottom = r.top + 16;

		BMenuBar *menubar = new BMenuBar(r, NULL);

		BMenu *menu = new BMenu("File", B_ITEMS_IN_COLUMN);
		menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'q', B_COMMAND_KEY));
		menubar->AddItem(menu);

		AddChild(menubar);
		menubar->ResizeToPreferred();

		BMenuItem *item;
		for(int32 k = 0; (item = menu->ItemAt(k)) != NULL; k++) item->SetTarget(BMessenger(this));

		frame.top = menubar->Frame().bottom + 1;
	}

	if(keys_count > 0) frame.bottom -= 30;

	VPDView *view = new VPDView(frame, "screen", B_FOLLOW_LEFT | B_FOLLOW_TOP);
	view->ResizeBuffer(w, h, cspace);
	view->SetPointSize(point_size);
	if(id >= 0)
	{
		BString str;
		str << id;

		view->SetLabel(str.String());
	}
	AddChild(view);

	view->ResizeToPreferred();
	if(view->Frame() != frame)
		ResizeBy(view->Frame().Width() - frame.Width(), view->Frame().Height() - frame.Height());

	if(keys_count == 0) return;

	r = Bounds();
	r.top = view->Frame().bottom + 1;
	r.right = r.left + r.Width() / (float)keys_count - 1.f;

	for(uint8 m = 0; m < keys_count; m++)
	{
		BString label;
		label << "K" << m + 1;

		AddChild(new VPDButton(r.InsetByCopy(5, 2), label.String(), (int8)m));

		r.OffsetBy(r.Width() + 1, 0);
	}
}


VPDWindow::~VPDWindow()
{
}


bool
VPDWindow::QuitRequested()
{
	if(fVPD != NULL)
	{
		fVPD->QuitRequested();
		fVPD = NULL;
	}
	return false;
}


void
VPDWindow::MessageReceived(BMessage *msg)
{
	BView *view;
	int8 keyID;
	bigtime_t when;
	bool keyState;

	switch(msg->what)
	{
		case VPD_MSG_KEY:
			if(fVPD == NULL) break;
			if(msg->FindInt8("id", &keyID) != B_OK || keyID < 0 || keyID >= 8) break;
			if(msg->FindBool("state", &keyState) != B_OK) break;
			if(msg->FindInt64("when", &when) != B_OK)
				when = real_time_clock_usecs();
			if(keyState)
				fVPD->KeyDown((uint8)keyID, when);
			else
				fVPD->KeyUp((uint8)keyID, when);
			break;

		case VPD_MSG_POWER_STATE:
		case VPD_MSG_ENABLE_UPDATE:
		case VPD_MSG_FILL_RECT:
		case VPD_MSG_INVERT_RECT:
		case VPD_MSG_DRAW_STRING:
		case VPD_MSG_STRING_WIDTH:
			if((view = FindView("screen")) == NULL) break;
			view->MessageReceived(msg);
			break;

		default:
			BWindow::MessageReceived(msg);
	}
}


VPDButton::VPDButton(BRect frame, const char *label, int8 id)
	: BButton(frame, NULL, label, NULL, B_FOLLOW_NONE),
	  fID(id),
	  fKeyMsgRunner(NULL)
{
}


VPDButton::~VPDButton()
{
	if(fKeyMsgRunner != NULL)
		delete fKeyMsgRunner;
}


void
VPDButton::ValueChanged()
{
	BMessage msg(VPD_MSG_KEY);
	msg.AddInt8("id", (int8)fID);
	msg.AddBool("state", Value() == B_CONTROL_ON);

	if(Value() == B_CONTROL_ON && fKeyMsgRunner == NULL)
	{
		BMessenger msgr(Looper(), Looper());
		fKeyMsgRunner = new BMessageRunner(msgr, &msg, 200000);
	}
	else if(Value() == B_CONTROL_OFF && fKeyMsgRunner != NULL)
	{
		delete fKeyMsgRunner;
		fKeyMsgRunner = NULL;

		msg.AddInt64("when", real_time_clock_usecs());
		Looper()->PostMessage(&msg);
	}
}


void
VPDButton::MouseDown(BPoint where)
{
	int32 v1, v2;

	v1 = Value();
	BButton::MouseDown(where);
	v2 = Value();

	if(v1 != v2) ValueChanged();
}


void
VPDButton::MouseUp(BPoint where)
{
	int32 v1, v2;

	v1 = Value();
	BButton::MouseUp(where);
	v2 = Value();

	if(v1 != v2) ValueChanged();
}


void
VPDButton::MouseMoved(BPoint where, uint32 code, const BMessage *msg)
{
	int32 v1, v2;

	v1 = Value();
	BButton::MouseMoved(where, code, msg);
	v2 = Value();

	if(v1 != v2) ValueChanged();
}


void
VPDButton::KeyDown(const char *bytes, int32 numBytes)
{
	int32 v1, v2;

	v1 = Value();
	BButton::KeyDown(bytes, numBytes);
	v2 = Value();

	if(v1 != v2) ValueChanged();
}


void
VPDButton::KeyUp(const char *bytes, int32 numBytes)
{
	int32 v1, v2;

	v1 = Value();
	BButton::KeyUp(bytes, numBytes);
	v2 = Value();

	if(v1 != v2) ValueChanged();
}

