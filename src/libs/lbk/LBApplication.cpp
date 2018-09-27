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
 * File: LBApplication.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>

#include <lbk/LBKConfig.h>
#include <lbk/LBApplication.h>

#include <lbk/add-ons/LBPanelDevice.h>

#if (1)
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

typedef enum
{
	LBK_ADD_ON_NONE_DEVICE = 0,
	LBK_ADD_ON_PANEL_DEVICE,
} LBKAddOnType;

typedef struct
{
	BString name;
	LBKAddOnType type;
	void *dev;
	void *image;
} LBKAddOnData;


static LBKAddOnData* lbk_app_load_panel_device_addon(const BPath &pth)
{
	LBKAddOnData *data = NULL;

#ifdef ETK_MAJOR_VERSION // use ETK++ function
	void *image = etk_load_addon(pth.Path());
	if(image == NULL) return NULL;

	LBPanelDevice* (*instantiate_func)() = NULL;
	LBPanelDevice *dev = NULL;

	if(etk_get_image_symbol(image, "instantiate_panel_device", (void**)&instantiate_func) != E_OK ||
	   (dev = (*instantiate_func)()) == NULL ||
	   dev->InitCheck() != B_OK)
	{
		if(dev != NULL) delete dev;
		etk_unload_addon(image);
		return NULL;
	}

	data = new LBKAddOnData;
	data->name.SetTo(pth.Leaf());
	data->type = LBK_ADD_ON_PANEL_DEVICE;
	data->dev = reinterpret_cast<void*>(dev);
	data->image = image;
#endif

	// TODO
	return data;
}


static void lbk_app_unload_panel_device_addon(LBKAddOnData *data)
{
#ifdef ETK_MAJOR_VERSION // use ETK++ function
	if(data == NULL || data->type != LBK_ADD_ON_PANEL_DEVICE) return;
	if(data->dev == NULL || data->image == NULL) return;

	LBPanelDevice *dev = reinterpret_cast<LBPanelDevice*>(data->dev);
	delete dev;

	etk_unload_addon(data->image);
#endif
}


static LBPanelDevice* lbk_app_find_first_panel_device(BList &addOnsList)
{
	for(int32 k = 0; k < addOnsList.CountItems(); k++)
	{
		LBKAddOnData *data = (LBKAddOnData*)addOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
			return reinterpret_cast<LBPanelDevice*>(data->dev);
	}

	return NULL;
}


LBApplication::LBApplication(const BList *cfg)
	: BLooper(NULL, B_URGENT_DISPLAY_PRIORITY),
	  fPulseRate(0),
	  fKeyState(0)
{
	bzero(fKeyTimestamps, sizeof(fKeyTimestamps));
	bzero(fKeyClicks, sizeof(fKeyClicks));

	fPipes[0] = -1;

	if(cfg != NULL)
	{
		for(int32 k = 0; k < cfg->CountItems(); k++)
		{
			BString *item = (BString*)cfg->ItemAt(k);
			if(item == NULL || item->Length() == 0) continue;
			if(item->FindFirst("#") == 0 || item->FindFirst("//") == 0) continue;
			item->RemoveAll("\r");
			item->RemoveAll("\n");

			int32 found = item->FindFirst("=");
			if(found <= 0) continue;

			BString name(item->String(), found);
			BString value(item->String() + found + 1);

			if(name == "PanelDeviceAddon")
			{
				BPath pth(value.String(), NULL, true);
				if(pth.Path() == NULL) continue;

				LBKAddOnData *data = lbk_app_load_panel_device_addon(pth);
				if(data == NULL)
				{
					fprintf(stderr,
						"[LBApplication]: %s --- Failed to load add-on (%s) !\n",
						__func__, pth.Path());
					continue;
				}
				fAddOnsList.AddItem(data);
			}

			// TODO
		}
	}
}


LBApplication::~LBApplication()
{
	LBView *view;
	while((view = (LBView*)fLeftPageViews.RemoveItem(0)) != NULL) delete view;
	while((view = (LBView*)fRightPageViews.RemoveItem(0)) != NULL) delete view;

	if(fPipes[0] >= 0)
	{
		close(fPipes[0]);
		close(fPipes[1]);
	}

	for(int32 k = 0; k < fAddOnsList.CountItems(); k++)
	{
		LBKAddOnData *data = (LBKAddOnData*)fAddOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
			lbk_app_unload_panel_device_addon(data);
		// TODO
		delete data;
	}
}


bool
LBApplication::AddPageView(LBView *view, bool left_side)
{
	LBPanelDevice *dev = lbk_app_find_first_panel_device(fAddOnsList);
	if(dev == NULL || view == NULL || view->Looper() != NULL || view->MasterView() != NULL) return false;

	if((left_side ? fLeftPageViews.AddItem(view) : fRightPageViews.AddItem(view)) == false) return false;
	AddHandler(view);

	view->fActivated = false;
	view->fDev = dev;

	view->Attached();

	return true;
}


bool
LBApplication::RemovePageView(LBView *view)
{
	if(view == NULL || view->Looper() != this || view->MasterView() != NULL) return false;

	view->Detached();
	RemoveHandler(view);

	if(fLeftPageViews.RemoveItem(view) == false)
		fRightPageViews.RemoveItem(view);

	if(view->fActivated)
		SetPreferredHandler(NULL);

	view->fActivated = false;
	view->fDev = NULL;

	return true;
}


LBView*
LBApplication::RemovePageView(int32 index, bool left_side)
{
	LBView *view = PageViewAt(index, left_side);

	return(RemovePageView(view) ? view : NULL);
}


LBView*
LBApplication::PageViewAt(int32 index, bool left_side) const
{
	return(left_side ? (LBView*)fLeftPageViews.ItemAt(index) : (LBView*)fRightPageViews.ItemAt(index));
}


int32
LBApplication::CountPageViews(bool left_side) const
{
	return(left_side ? fLeftPageViews.CountItems() : fRightPageViews.CountItems());
}

void
LBApplication::ActivatePageView(int32 index, bool left_side)
{
	LBView *newView = PageViewAt(index, left_side);
	LBView *oldView = GetActivatedPageView();

	if(oldView == newView || newView == NULL) return;

	if(oldView)
		oldView->SetActivated(false);

	SetPreferredHandler(newView);
	newView->SetActivated(true);
}


LBView*
LBApplication::GetActivatedPageView() const
{
	// ignore NULL to make it compatible with old BeOS API
	return(PreferredHandler() ? cast_as(PreferredHandler(), LBView) : NULL);
}


void
LBApplication::Go()
{
	if(IsRunning())
	{
		printf("[LBApplication]: It's forbidden to run Go() more than ONE time !\n");
		return;
	}

	LBPanelDevice* dev = lbk_app_find_first_panel_device(fAddOnsList);
	if(dev == NULL)
	{
		printf("[LBApplication]: NO PANEL DEVICE !\n");
		return;
	}
	dev->fMsgr = BMessenger(this, this);

	if(pipe(fPipes) < 0)
	{
		perror("[LBApplication]: Unable to create pipe");
		return;
	}

	Lock();
	Run();
	ActivatePageView(0, false);
	Unlock();

	fd_set rset;
	struct timeval timeout;
	uint32 count = 0;
	bigtime_t pulse_sent_time[2] = {0, 0};
	bigtime_t pulse_rate = 0;

	timeout.tv_sec = 0;
	while(IsRunning())
	{
		timeout.tv_usec = (pulse_rate > 0 && pulse_rate < (bigtime_t)500000) ? pulse_rate : 500000;
		if(count > 0 && timeout.tv_usec > LBK_KEY_INTERVAL / 3)
			timeout.tv_usec = LBK_KEY_INTERVAL / 3;

		FD_ZERO(&rset);
		FD_SET(fPipes[0], &rset);
		int status = select(fPipes[0] + 1, &rset, NULL, NULL, &timeout);
		if(status < 0)
		{
			perror("[LBApplication]: Unable to get event from input device");
			break;
		}

		if(status > 0 && FD_ISSET(fPipes[0], &rset))
		{
			uint8 byte = 0x00;
			if(read(fPipes[0], &byte, 1) == 1)
			{
				switch(byte)
				{
					case 0xab:
						count++;
						break;

					default:
						pulse_rate = this->PulseRate();
				}
			}
		}

		if(count > 0 && real_time_clock_usecs() - pulse_sent_time[0] >= (bigtime_t)(LBK_KEY_INTERVAL / 2))
		{
			count--;
			Lock();
			PostMessage(B_PULSE, this);
			Unlock();
			pulse_sent_time[0] = real_time_clock_usecs();
		}

		if(pulse_rate > 0 && real_time_clock_usecs() - pulse_sent_time[1] >= pulse_rate)
		{
			BMessage aMsg(B_PULSE);
			aMsg.AddBool("no_button_check", true);

			Lock();
			PostMessage(&aMsg, this);
			Unlock();
			pulse_sent_time[1] = real_time_clock_usecs();
		}
	}

	Lock();
	PostMessage(B_QUIT_REQUESTED, this);
	Unlock();
}


void
LBApplication::MessageReceived(BMessage *msg)
{
	bigtime_t when;
	uint8 nKey = 0;
	bool stopRunner;

	switch(msg->what)
	{
		case B_KEY_DOWN:
		case B_KEY_UP:
			if(PreferredHandler() == NULL) break;
			if(msg->FindInt8("key", (int8*)&nKey) != B_OK || nKey >= 8) break;
			if(msg->FindInt64("when", &when) != B_OK) break;
			if(msg->what == B_KEY_DOWN)
			{
				if((fKeyState & (0x01 << nKey)) != 0) // already DOWN
				{
					// auto-repeat (event.value = 2) event
					if(when < fKeyTimestamps[nKey]) break;
					if(when - fKeyTimestamps[nKey] < (bigtime_t)500000) break; // 0.5s
					if(fKeyClicks[nKey] == 0xff) break;
					fKeyClicks[nKey] = 0xff; // long press
				}
				else
				{
					if(fKeyClicks[nKey] > 0 && when < fKeyTimestamps[nKey]) break;
					if(fKeyClicks[nKey] < 0xff) fKeyClicks[nKey]++;
				}
				fKeyTimestamps[nKey] = when;

				BMessage aMsg(B_KEY_DOWN);
				aMsg.AddInt8("key", *((int8*)&nKey));
				aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[nKey]));
				aMsg.AddInt64("when", when);
				PostMessage(&aMsg, PreferredHandler());

				fKeyState |= (0x1 << nKey);
			}
			else
			{
				uint8 byte = 0xab;

				if((fKeyState & (0x01 << nKey)) == 0) break; // already UP
				if(when < fKeyTimestamps[nKey]) break;
				if(write(fPipes[1], &byte, 1) <= 0)
				{
					DBGOUT("[LBApplication]: Failed to notice the main thread.\n");
					BMessage aMsg(B_KEY_UP);
					aMsg.AddInt8("key", *((int8*)&nKey));
					aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[nKey]));
					aMsg.AddInt64("when", when);
					PostMessage(&aMsg, PreferredHandler());

					fKeyState &= ~(0x01 << nKey);
					fKeyClicks[nKey] = 0;
					fKeyTimestamps[nKey] = 0;
				}
				else
				{
					fKeyState &= ~(0x01 << nKey);
					fKeyTimestamps[nKey] = when;
				}
			}
			break;

		case B_PULSE:
			DBGOUT("[LBApplication]: B_PULSE received.\n");
			if(msg->HasBool("no_button_check"))
			{
				if(PreferredHandler() != NULL)
					PostMessage(B_PULSE, PreferredHandler());
				break;
			}
			stopRunner = true;
			when = real_time_clock_usecs();
			nKey = CountKeys(0);
			for(uint8 k = 0; k < nKey; k++)
			{
				if((fKeyState & (0x01 << k)) != 0) continue; // DOWN, no need
				if(fKeyClicks[k] == 0 || fKeyTimestamps[k] == 0) continue; // no UP before
				if(when < fKeyTimestamps[k]) continue; // should never happen
				if(when - fKeyTimestamps[k] < (bigtime_t)LBK_KEY_INTERVAL)
				{
					stopRunner = false;
					continue;
				}

				if(PreferredHandler() != NULL)
				{
					BMessage aMsg(B_KEY_UP);
					aMsg.AddInt8("key", *((int8*)&k));
					aMsg.AddInt8("clicks", *((int8*)&fKeyClicks[k]));
					aMsg.AddInt64("when", fKeyTimestamps[k]);
					PostMessage(&aMsg, PreferredHandler());
				}

				fKeyState &= ~(0x01 << k);
				fKeyClicks[k] = 0;
				fKeyTimestamps[k] = 0;
			}
			if(stopRunner == false)
			{
				uint8 byte = 0xab;
				if(write(fPipes[1], &byte, 1) <= 0)
				{
					DBGOUT("[LBApplication]: Failed to notice the main thread.\n");
					PostMessage(B_PULSE, this); // try again
				}
			}
			break;

		default:
			BLooper::MessageReceived(msg);
	}
}


bigtime_t
LBApplication::PulseRate() const
{
	return fPulseRate;
}


void
LBApplication::SetPulseRate(bigtime_t rate)
{
	if(rate != 0 && rate < (bigtime_t)50000)
		rate = (bigtime_t)50000;
	else if(rate > (bigtime_t)10000000)
		rate = (bigtime_t)10000000;

	if(fPulseRate != rate)
	{
		fPulseRate = rate;

		if(fPipes[1] >= 0)
		{
			uint8 byte = 0x01;
			write(fPipes[1], &byte, 1);
		}
	}
}


uint8
LBApplication::CountKeys(int32 indexPanel) const
{
	uint8 n = 0;

	if(indexPanel < 0 || indexPanel >= fAddOnsList.CountItems()) return 0;

	for(int32 k = 0; k < fAddOnsList.CountItems(); k++)
	{
		LBKAddOnData *data = (LBKAddOnData*)fAddOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
		{
			if(indexPanel-- == 0)
			{
				LBPanelDevice *dev = reinterpret_cast<LBPanelDevice*>(data->dev);
				dev->GetCountOfKeys(n);
				break;
			}
		}
	}

	return n;
}

