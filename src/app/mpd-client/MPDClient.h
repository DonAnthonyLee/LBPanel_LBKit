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
 * File: MPDClient.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __MPD_CLIENT_H__
#define __MPD_CLIENT_H__

#include <be/Be.h>

#ifdef __cplusplus /* Just for C++ */

class MPDClient {
public:
	MPDClient();
	~MPDClient();

	status_t	Connect(const char *address, uint16 port);

	status_t	GetStatus(BMessage *msg);
	status_t	GetCurrentSongInfo(BMessage *msg);
	void		NextSong();
	void		PrevSong();
	void		Pause(bool pause = true);
	void		Play();
	void		Stop();

	void		KeepAlive();
	const char*	GetMPDVersion(uint8 *major = NULL,
				      uint8 *minor = NULL,
				      uint8 *micro = NULL) const;

	void		SetTimeout(bigtime_t t);

private:
	BNetEndpoint fEndpoint;
	BString fMPDVersion;
	bigtime_t fTimeout;

	status_t	SendCommand(const char *cmd, BString *recvBuf = NULL);
	void		ConvertToMessage(const BString *recvBuf, BMessage *msg);
};

inline void MPDClient::KeepAlive()
{
	GetStatus(NULL);
}

#endif /* __cplusplus */

#endif /* __MPD_CLIENT_H__ */

