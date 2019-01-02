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
 * File: LBPanelKeypadGeneric.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKConfig.h>

#ifdef ETK_OS_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <stdio.h>
#endif

#include <lbk/add-ons/LBPanelKeypadGeneric.h>

#if (0)
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif


LBPanelKeypadGeneric::LBPanelKeypadGeneric(const char *dev)
	: LBPanelKeypad(),
	  fFD(-1),
	  fThread(NULL),
	  fBlockKeyEvents(false),
	  fBlockTimestamp(0)
{
	bzero(&fKeycodes, sizeof(fKeycodes));
	fPipes[0] = fPipes[1] = -1;

#ifdef ETK_OS_LINUX
	if ((fFD = open(dev, O_RDONLY)) < 0)
	{
		perror("[LBPanelKeypadGeneric]: Unable to open device !");
		return;
	}

	if(pipe(fPipes) < 0)
	{
		perror("[LBPanelKeypadGeneric]: Unable to create pipe !");
		return;
	}
#else
	// TODO
	fprintf(stderr, "[LBPanelKeypadGeneric]: No implementation of device !");
	return
#endif // ETK_OS_LINUX

#ifdef ETK_MAJOR_VERSION
	if((fThread = etk_create_thread(this->InputEventsObserver,
					E_URGENT_DISPLAY_PRIORITY,
					reinterpret_cast<void*>(this),
					NULL)) == NULL ||
	   etk_resume_thread(fThread) != E_OK)
	{
		if(fThread != NULL)
		{
			etk_delete_thread(fThread);
			fThread = NULL;
		}
		ETK_WARNING("[LBPanelKeypadGeneric]: Unable to create thread !\n");
	}
#else
	thread_id tid = spawn_thread(this->InputEventsObserver,
				     "lbk_keypad_generic",
				     B_URGENT_DISPLAY_PRIORITY,
				     reinterpret_cast<void*>(this));
	if(tid < 0 || resume_thread(tid) != B_OK)
	{
		fprintf(stderr, "[LBPanelKeypadGeneric]: Unable to spawn thread !\n");
	}
	else
	{
		fThread = reinterpret_cast<void*>((long)tid);
	}
#endif // ETK_MAJOR_VERSION
}


LBPanelKeypadGeneric::~LBPanelKeypadGeneric()
{
#ifdef ETK_MAJOR_VERSION
	if(fThread != NULL)
	{
#ifdef ETK_OS_LINUX
		if(fPipes[0] >= 0)
		{
			e_status_t status;
			uint8 t = 0xff;
			write(fPipes[1], &t, 1);
			etk_wait_for_thread(fThread, &status);
		}
#endif // ETK_OS_LINUX
		etk_delete_thread(fThread);
	}
#else
	thread_id tid = (fThread == NULL ? 0 : (thread_id)reinterpret_cast<long>(fThread));
	if(tid > 0)
	{
		// TODO

		status_t status;
		wait_for_thread(tid, &status);
	}
#endif // ETK_MAJOR_VERSION

#ifdef ETK_OS_LINUX
	if(fFD >= 0) close(fFD);
	if(fPipes[0] >= 0)
	{
		close(fPipes[0]);
		close(fPipes[1]);
	}
#endif // ETK_OS_LINUX
}


status_t
LBPanelKeypadGeneric::InitCheck(const char *options)
{
	return((fFD < 0 || fPipes[0] < 0 || fThread == NULL) ? B_ERROR : B_OK);
}


void
LBPanelKeypadGeneric::SetKeycode(int32 id, uint16 keycode)
{
	if(id < 0 || id >= (int32)(sizeof(fKeycodes) / sizeof(fKeycodes[0]))) return;
	fKeycodes[id] = keycode;
}


status_t
LBPanelKeypadGeneric::GetCountOfKeys(uint8 &count)
{
	if(fFD < 0) return B_ERROR;

	count = 0;
	for(size_t k = 0; k < sizeof(fKeycodes) / sizeof(fKeycodes[0]); k++)
	{
		if(fKeycodes[k] > 0) count++;
	}

	return B_OK;
}


status_t
LBPanelKeypadGeneric::BlockKeyEvents(bool state)
{
	if(Panel() == NULL) return B_ERROR;

#ifdef ETK_MAJOR_VERSION
	EAutolock <LBPanelDevice>autolock(Panel());
#else
	BAutolock autolock(Panel());
#endif
	if(autolock.IsLocked() == false) return B_ERROR;

	if(fBlockKeyEvents != state)
	{
		fBlockKeyEvents = state;

		if(fBlockKeyEvents == false)
			fBlockTimestamp = system_time();
	}

	return B_OK;
}

#ifdef ETK_OS_LINUX
int32
LBPanelKeypadGeneric::InputEventsObserver(void *arg)
{
	LBPanelKeypadGeneric *self = reinterpret_cast<LBPanelKeypadGeneric*>(arg);

	fd_set rset;
	int input_fd = self->fFD;
	int pipe_fd = self->fPipes[0];
	bool doingGood = (input_fd < 0 || pipe_fd < 0) ? false : true;

	while(doingGood)
	{
		FD_ZERO(&rset);
		FD_SET(input_fd, &rset);
		FD_SET(pipe_fd, &rset);

		int status = select(max_c(input_fd, pipe_fd) + 1, &rset, NULL, NULL, NULL);
		if(status < 0)
		{
			perror("[LBPanelKeypadGeneric]: Unable to get event from input device");
			break;
		}

		if(status > 0 && FD_ISSET(pipe_fd, &rset))
		{
			uint8 byte = 0x00;
			doingGood = !(read(pipe_fd, &byte, 1) != 1 || byte == 0xff);
		}

		if(status == 0) continue;

		if(!FD_ISSET(input_fd, &rset)) continue;

		struct input_event event;
		int n = read(input_fd, &event, sizeof(event));

		if(n <= 0)
		{
			perror("[LBPanelKeypadGeneric]: Unable to get event from input device");
			break;
		}

		if(n != sizeof(event))
		{
			printf("[LBPanelKeypadGeneric]: Unable to process input event !\n");
			continue;
		}

		if(event.type != EV_KEY)
		{
			DBGOUT("[LBPanelKeypadGeneric]: event.type(%u) != EV_KEY.\n", event.type);
			continue;
		}

		// NOTE: etk_system_boot_time() requires the version of ETK++ >= 0.4.4
		bigtime_t when = (bigtime_t)event.time.tv_sec * (bigtime_t)(1000000) +
				 (bigtime_t)event.time.tv_usec - etk_system_boot_time();

		if(self->Panel() == NULL || self->Panel()->Lock() == false) continue;
		if(self->fBlockKeyEvents || self->fBlockTimestamp > when)
		{
			self->Panel()->Unlock();
			continue;
		}
#if 0
		bigtime_t t;
		self->SetTimestampNow(t, false); // update screen's timestamp
#endif
		self->Panel()->Unlock();

		uint8 nKey = 0xff;
		for(size_t k = 0; k < sizeof(fKeycodes) / sizeof(fKeycodes[0]); k++)
		{
			if(self->fKeycodes[k] == event.code)
			{
				nKey = (uint8)k;
			}
		}
		if(nKey == 0xff) continue;

		BMessage msg(event.value == 0 ? B_KEY_UP : B_KEY_DOWN);
		msg.AddInt8("key", *((int8*)&nKey));
		msg.AddInt64("when", when);

		self->SendMessageToApp(&msg);
	}

	self->SendMessageToApp(LBK_QUIT_REQUESTED);
	return 0;
}
#else
int32
LBPanelKeypadGeneric::InputEventsObserver(void *arg)
{
	// TODO
	return -1;
}
#endif

