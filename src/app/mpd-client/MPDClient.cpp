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
 * File: MPDClient.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <string.h>

#include "MPDClient.h"


MPDClient::MPDClient()
	: fTimeout(500000)
{
	// TODO
	fEndpoint.SetTimeout(0);
}


MPDClient::~MPDClient()
{
	// TODO
}


void
MPDClient::SetTimeout(bigtime_t t)
{
	fTimeout = t;
}


status_t
MPDClient::Connect(const char *address, uint16 port)
{
	status_t st = fEndpoint.Connect(address, port);

	if(st == B_OK && SendCommand(NULL, &fMPDVersion) == B_OK)
	{
		if(fMPDVersion.FindFirst("OK MPD ") != 0)
		{
			fMPDVersion.Truncate(0);
			fEndpoint.Close();
			st = B_ERROR;
		}
		else
		{
			int32 found = fMPDVersion.FindFirst("\n");
			if(found > 0) fMPDVersion.Truncate(found);
			fMPDVersion.Remove(0, 7);
		}
	}
	else
	{
		fEndpoint.Close();
		st = B_ERROR;
	}

	return st;
}


void
MPDClient::ConvertToMessage(const BString *recvBuf, BMessage *msg)
{
	BString buf;

	if(msg == NULL) return;
	if(recvBuf != NULL) buf.SetTo(recvBuf->String());

	while(buf.Length() > 1)
	{
		BString str, value;

		int32 found = buf.FindFirst("\n");
		if(found < 0)
			found = buf.Length();

		str.SetTo(buf.String(), found);
		buf.Remove(0, min_c(buf.Length(), found + 1));

		if((found = str.FindFirst(": ")) <= 0) continue;
		value.SetTo(str.String() + found + 2);
		if(value.Length() == 0) continue;
		str.Truncate(found);
		msg->AddString(str.String(), value);
	}
}


status_t
MPDClient::GetCurrentSongInfo(BMessage *msg)
{
	BString buf;

	if(SendCommand("currentsong\n", &buf) != B_OK) return B_ERROR;
	if(msg == NULL) return B_BAD_VALUE;

	ConvertToMessage(&buf, msg);

	return B_OK;
}


status_t
MPDClient::GetStatus(BMessage *msg)
{
	BString buf;

	if(SendCommand("status\n", &buf) != B_OK) return B_ERROR;
	if(msg == NULL) return B_BAD_VALUE;

	ConvertToMessage(&buf, msg);
	if(msg->HasString("songid")) GetCurrentSongInfo(msg);

	return B_OK;
}


void
MPDClient::NextSong()
{
	BString buf;
	SendCommand("next\n", &buf);

	// TODO
}


void
MPDClient::PrevSong()
{
	BString buf;
	SendCommand("previous\n", &buf);

	// TODO
}


void
MPDClient::Pause(bool pause)
{
	BString cmd, buf;
	cmd << "pause " << (pause ? "1" : "0") << "\n";

	SendCommand(cmd.String(), &buf);

	// TODO
}


void
MPDClient::Play()
{
	BString buf;
	SendCommand("play\n", &buf);

	// TODO
}


void
MPDClient::Stop()
{
	BString buf;
	SendCommand("stop\n", &buf);

	// TODO
}


const char*
MPDClient::GetMPDVersion(uint8 *major, uint8 *minor, uint8 *micro) const
{
	if(fEndpoint.InitCheck() != B_OK) return NULL;

	if(major != NULL || minor != NULL || micro != NULL)
	{
		uint8 ver[3] = {0, 0, 0};

		int32 offset = 0;
		for(int k = 0; k < 3 && offset < fMPDVersion.Length(); k++)
		{
			int32 found = fMPDVersion.FindFirst(".", offset);
			if(found < 0) found = fMPDVersion.Length();

			BString str(fMPDVersion.String() + offset, found - offset);
			ver[k] = (uint8)atoi(str.String());

			offset = found + 1;
		}

		if(major != NULL) *major = ver[0];
		if(minor != NULL) *minor = ver[1];
		if(micro != NULL) *micro = ver[2];

	}

	return fMPDVersion.String();
}


status_t
MPDClient::SendCommand(const char *cmd, BString *recvBuf)
{
	status_t retVal = B_OK;
	char buf[1024];

	if(fEndpoint.InitCheck() != B_OK) return B_ERROR;
	if(cmd == NULL && recvBuf == NULL) return B_BAD_VALUE;

	if(cmd != NULL)
	{
		if(*cmd == 0) return B_BAD_VALUE;

		fEndpoint.SetNonBlocking(true);
		while(fEndpoint.IsDataPending(0)) // clear data
			fEndpoint.Receive(buf, sizeof(buf));
		fEndpoint.SetNonBlocking(false);

		size_t len = strlen(cmd);
		if(fEndpoint.Send(cmd, len) != (int32)len) return B_ERROR;
	}

	if(recvBuf != NULL)
	{
		recvBuf->Truncate(0);

		bigtime_t when = system_time() + fTimeout;
		bigtime_t t = (fTimeout > 0) ? fTimeout : 0;

		while(t >= 0 && fEndpoint.IsDataPending(t))
		{
			if(recvBuf->Length() >= 65535) // 64kB
			{
				retVal = B_WOULD_BLOCK;
				break;
			}

			bzero(buf, sizeof(buf));
			fEndpoint.SetNonBlocking(true);
			if(fEndpoint.Receive(buf, sizeof(buf)) < 0) break;

			if(buf[0] != 0)
				recvBuf->Append(buf);

			t = when - system_time();
			if(fEndpoint.IsDataPending(0) == false) break;
		}

		if(recvBuf->Length() == 0)
			retVal = B_TIMED_OUT;

		fEndpoint.SetNonBlocking(false);
	}

	return retVal;
}

