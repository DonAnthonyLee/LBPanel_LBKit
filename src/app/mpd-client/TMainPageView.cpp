/* --------------------------------------------------------------------------
 *
 * Simple MPD client using LBKit
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
 * File: TMainPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "IconDefs.h"
#include "TMainPageView.h"

#define MPD_STATE_PLAY		0x01
#define MPD_STATE_PAUSE		0x02
#define MPD_STATE_STOP		0x04
#define MPD_STATE_REPEAT	0x08
#define MPD_STATE_RANDOM	0x10
#define MPD_STATE_ERROR		0x80


TMainPageView::TMainPageView(const char *name)
	: LBPageView(name),
	  fSongNamePos(0), fSongTime(0), fCurrSongTime(0),
	  fPlayState(0)
{
	SetNavButtonIcon(0, LBK_ICON_ID_16x16_USER1);
	SetNavButtonIcon(1, LBK_ICON_ID_16x16_USER2);
	SetNavButtonIcon(2, LBK_ICON_ID_16x16_USER3);

	ShowNavButton(0);
	ShowNavButton(1);
	ShowNavButton(2);

	// TODO: read config
	if(fClient.Connect("localhost", 6600) != B_OK)
	{
		fPlayState |= MPD_STATE_ERROR;
	}
}


TMainPageView::~TMainPageView()
{
	// TODO
}


void
TMainPageView::Pulse()
{
	BMessage msg;
	if(fClient.GetStatus(&msg) != B_OK) return;

#if 0
	msg.PrintToStream();
#endif

	uint32 song_time = 0;
	uint32 curr_song_time = 0;
	const char *item;
	uint8 state = 0;
	BString str;

	// Play State
	if(msg.HasString("error"))
		state |= MPD_STATE_ERROR;
	if(msg.FindString("state", &item) == B_OK && item != NULL)
	{
		if(strcmp(item, "play") == 0)
			state |= MPD_STATE_PLAY;
		else if(strcmp(item, "pause") == 0)
			state |= MPD_STATE_PAUSE;
		else if(strcmp(item, "stop") == 0)
			state |= MPD_STATE_STOP;
		else
			state |= MPD_STATE_ERROR;
	}
	if(msg.FindString("repeat", &item) == B_OK && item != NULL)
	{
		if(*item == '1')
			state |= MPD_STATE_REPEAT;
	}
	if(msg.FindString("random", &item) == B_OK && item != NULL)
	{
		if(*item == '1')
			state |= MPD_STATE_RANDOM;
	}

	if(state != fPlayState)
	{
		fPlayState = state;
		if(state & (MPD_STATE_STOP | MPD_STATE_PAUSE))
			SetNavButtonIcon(1, LBK_ICON_ID_16x16_USER2); // show icon_play
		else
			SetNavButtonIcon(1, LBK_ICON_ID_16x16_USER4); // show icon_pause
		UpdatePlayState();
	}

	// Song time
	if(msg.FindString("time", &item) == B_OK)
	{
		str.SetTo(item);
		int32 found = str.FindFirst(":");
		if(found > 0)
		{
			song_time = (uint32)atol(str.String() + found + 1);
			str.Truncate(found);
			curr_song_time = (uint32)atol(str.String());
		}
	}
	if(song_time != fSongTime || curr_song_time != fCurrSongTime)
	{
		fSongTime = song_time;
		fCurrSongTime = curr_song_time;

		BRect r = Bounds();
		r.top = r.bottom - 15;
		Invalidate(r);
	}

	// Song name
	str.Truncate(0);
	if(msg.FindString("Title", &item) == B_OK)
	{
		str.SetTo(item);
	}
	else if(msg.FindString("file", &item) == B_OK)
	{
		str.SetTo(item);
		int32 found = str.FindLast("/");
		if(found >= 0)
			str.Remove(0, found + 1);
		found = str.FindLast(".");
		if(found > 0 && found > str.Length() - 5)
			str.Truncate(found);
	}

	if(str != fSongName)
	{
		fSongName = str;
		fSongNamePos = 0;
	}
	UpdateSongNamePosIfNeeded();
}


void
TMainPageView::UpdatePlayState()
{
	// TODO
}


void
TMainPageView::UpdateSongNamePosIfNeeded()
{
	if(fSongNamePos < 0) return;

	SetFontSize(12);

	BRect r = Bounds();
	if(fSongNamePos == 0 && fSongName.Length() > 0)
	{
		if(StringWidth(fSongName.String()) <= r.Width() - 4)
			fSongNamePos = -1;
		else
			fSongNamePos = 1;
	}
	else if(fSongNamePos > 0)
	{
		fSongNamePos += 2;
		if(fSongNamePos > StringWidth(fSongName.String()))
			fSongNamePos = 1;
	}

	r.bottom = r.top + 15;
	Invalidate(r);
}


void
TMainPageView::Draw(BRect updateRect)
{
	LBPageView::Draw(updateRect);

	// Song name
	BRect r = Bounds();
	r.bottom = r.top + 16;
	if(updateRect.Intersects(r) && fSongName.Length() > 0)
	{
		SetFontSize(12);

		BPoint pt = r.LeftTop() + BPoint(2, 2);
		if(fSongNamePos > 1)
			pt.x -= fSongNamePos - 1;

		bigtime_t ts;
		PanelDevice()->ConstrainClipping(r.InsetByCopy(2, 2), ts);
		DrawString(fSongName.String(), pt);
		PanelDevice()->ConstrainClipping(BRect(), ts);
		SetPanelDeviceTimestamp(ts); // avoid whole updateing
	}

	// Song time
	r = Bounds();
	r.top = r.bottom - 15;
	r.InsetBy(2, 0);
	if(updateRect.Intersects(r))
	{
		char buf[256];
		SetFontSize(8);

		// Current song time
		bzero(buf, sizeof(buf));
		snprintf(buf, sizeof(buf), "%02d:%02d",
			 fCurrSongTime / 60,
			 fCurrSongTime % 60);
		DrawString(buf, r.LeftTop());

		// Song time
		bzero(buf, sizeof(buf));
		snprintf(buf, sizeof(buf), "%02d:%02d",
			 fSongTime / 60,
			 fSongTime % 60);
		DrawString(buf, r.RightTop() - BPoint(StringWidth(buf), 0));

		// bar
		r.top += 10;
		StrokeRect(r);
		if(fSongTime > 0)
		{
			r.InsetBy(1, 1);
			r.right = r.left + r.Width() * (float)fCurrSongTime/(float)fSongTime;
			FillRect(r);
		}
	}
}


void
TMainPageView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	// TODO: first/last song
	if(clicks == 0xff) switch(key)
	{
		case 1: // Stop
			fClient.Stop();
			Pulse();
			break;

		default:
			break;
	}
}


void
TMainPageView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	if(clicks == 1) switch(key)
	{
		case 0: // prev song
			fClient.PrevSong();
			Pulse();
			break;

		case 1: // Play/Pause
			if(fPlayState & MPD_STATE_PLAY)
				fClient.Pause();
			else if(fPlayState & MPD_STATE_PAUSE)
				fClient.Pause(false);
			else
				fClient.Play();
			Pulse();
			break;

		case 2: // next song
			fClient.NextSong();
			Pulse();
			break;

		default:
			break;
	}
}


void
TMainPageView::Activated(bool state)
{
	// TODO
	LBPageView::Activated(state);
	cast_as(Looper(), LBApplication)->SetPulseRate(state ? 1000000 : 0);
}


void
TMainPageView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		// TODO
		default:
			LBPageView::MessageReceived(msg);
	}
}


void
TMainPageView::DrawNavButtonIcon(lbk_icon_id idIcon, BPoint location)
{
	switch(idIcon)
	{
		case LBK_ICON_ID_16x16_USER1:
			DrawIcon(&icon_prev, location);
			break;

		case LBK_ICON_ID_16x16_USER2:
			DrawIcon(&icon_play, location);
			break;

		case LBK_ICON_ID_16x16_USER3:
			DrawIcon(&icon_next, location);
			break;

		case LBK_ICON_ID_16x16_USER4:
			DrawIcon(&icon_pause, location);
			break;

		default:
			DrawIcon(idIcon, location);
	}
}

