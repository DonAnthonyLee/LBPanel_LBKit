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
#include <unistd.h>
#include <sys/select.h>

#include <lbk/LBKConfig.h>
#include <lbk/LBApplication.h>

#include <lbk/add-ons/LBPanelDevice.h>

#if (0)
#define DBGOUT(msg...)		do { printf(msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#ifdef LBK_APP_IPC_BY_FIFO // defined in "<lbk/LBKConfig.h>"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct
{
	BString path;
	int fd;
} LBKAppIPC;
#endif // LBK_APP_IPC_BY_FIFO

typedef enum
{
	LBK_ADD_ON_NONE = 0,
	LBK_ADD_ON_PANEL_DEVICE,
} LBAddOnType;

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

typedef struct
{
	uint8 keyState;
	bigtime_t keyTimestamps[8];
	uint8 keyClicks[8];
	BList *leftPageViews;
	BList *rightPageViews;
	BHandler *preferHandler;
	bool valid;
} LBPanelDeviceAddOnData;

typedef struct
{
	BString name;
	LBAddOnType type;
	void *dev;
	image_id image;
	void *data;
} LBAddOnData;


static LBAddOnData* lbk_app_load_panel_device_addon(const BPath &pth, const char *opt)
{
	LBAddOnData *data = NULL;

	image_id image = load_add_on(pth.Path());
	if(!IMAGE_IS_VALID(image)) return NULL;

	LBPanelDevice* (*instantiate_func)() = NULL;
	LBPanelDevice *dev = NULL;
	LBPanelDeviceAddOnData *devData = NULL;

	if(get_image_symbol(image, "instantiate_panel_device", B_SYMBOL_TYPE_TEXT, (void**)&instantiate_func) != B_OK ||
	   (dev = (*instantiate_func)()) == NULL ||
	   dev->InitCheck(opt) != B_OK ||
	   (devData = (LBPanelDeviceAddOnData*)malloc(sizeof(LBPanelDeviceAddOnData))) == NULL)
	{
		if(dev != NULL) delete dev;
		unload_add_on(image);
		return NULL;
	}

	data = new LBAddOnData;
	data->name.SetTo(pth.Leaf());
	data->type = LBK_ADD_ON_PANEL_DEVICE;
	data->dev = reinterpret_cast<void*>(dev);
	data->image = image;

	bzero(devData, sizeof(LBPanelDeviceAddOnData));
	devData->leftPageViews = new BList();
	devData->rightPageViews = new BList();
	devData->valid = true;
	data->data = devData;

	return data;
}


static void lbk_app_unload_panel_device_addon(LBAddOnData *data)
{
	if(data == NULL || data->type != LBK_ADD_ON_PANEL_DEVICE) return;
	if(data->dev == NULL || !IMAGE_IS_VALID(data->image)) return;

	LBPanelDevice *dev = reinterpret_cast<LBPanelDevice*>(data->dev);
	delete dev;

	LBPanelDeviceAddOnData *devData = (LBPanelDeviceAddOnData*)data->data;
	delete devData->leftPageViews;
	delete devData->rightPageViews;
	free(devData);
}


static LBPanelDevice* lbk_app_get_panel_device(const BList &addOnsList, int32 index)
{
	if(index < 0 || index >= addOnsList.CountItems()) return NULL;

	for(int32 k = 0; k < addOnsList.CountItems(); k++)
	{
		LBAddOnData *data = (LBAddOnData*)addOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
		{
			if(index-- == 0)
				return(reinterpret_cast<LBPanelDevice*>(data->dev));
		}
	}

	return NULL;
}


static LBPanelDeviceAddOnData* lbk_app_get_panel_device_data(const BList &addOnsList, int32 index)
{
	if(index < 0 || index >= addOnsList.CountItems()) return NULL;

	for(int32 k = 0; k < addOnsList.CountItems(); k++)
	{
		LBAddOnData *data = (LBAddOnData*)addOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
		{
			if(index-- == 0)
				return((LBPanelDeviceAddOnData*)data->data);
		}
	}

	return NULL;
}


LBApplication::LBApplication(const BList *cfg)
	: BLooper(NULL, B_URGENT_DISPLAY_PRIORITY),
	  fQuitLooper(false),
	  fPulseRate(0),
	  fPanelsCount(0),
	  fIPC(NULL)
{
	fPipes[0] = -1;

	if(cfg != NULL)
	{
		for(int32 k = 0; k < cfg->CountItems(); k++)
		{
			BString *item = (BString*)cfg->ItemAt(k);
			if(item == NULL || item->Length() == 0) continue;
			if(item->ByteAt(0) == '#' || item->FindFirst("//") == 0) continue;
			item->RemoveAll("\r");
			item->RemoveAll("\n");
			item->RemoveAll(" ");

			int32 found = item->FindFirst("=");
			if(found <= 0 || found == item->Length() - 1) continue;

			BString name(item->String(), found);
			BString value(item->String() + found + 1);
			BString options;

			found = value.FindFirst(",");
			if(found > 0)
			{
				if(found < value.Length() - 1)
					options.SetTo(value.String() + found + 1);
				value.Truncate(found);
			}

			if(name == "PanelDeviceAddon")
			{
				BPath pth(value.String(), NULL, true);
				if(pth.Path() == NULL) continue;

				LBAddOnData *data = lbk_app_load_panel_device_addon(pth, options.String());
				if(data == NULL)
				{
					fprintf(stderr,
						"[LBApplication]: %s --- Failed to load add-on (%s) !\n",
						__func__, pth.Path());
					continue;
				}

				fPanelsCount++;
				fAddOnsList.AddItem(data);
			}
			else if(name == "IPC" && fIPC == NULL)
			{
#ifdef LBK_APP_IPC_BY_FIFO
				int fd;
				BPath pth;
				BString str;
				str << "/tmp/lbk_ipc_" << getuid();

				BEntry entry(str.String());
				entry.GetPath(&pth);
				if(!(entry.Exists() && entry.IsDirectory()))
				{
					if(mkdir(pth.Path(), 0700) != 0)
					{
						fprintf(stderr, "[LBApplication]: %s --- Failed to initialize IPC !\n", __func__);
						continue;
					}
				}

				entry.SetTo(pth.Path(), value);
				entry.GetPath(&pth);

				unlink(pth.Path());
				if(mkfifo(pth.Path(), 0600) == -1 ||
				   chmod(pth.Path(), 0600) == -1 ||
				   (fd = open(pth.Path(), O_NONBLOCK | O_RDONLY)) < 0) {
					fprintf(stderr,
						"[LBApplication]: %s --- Failed to create fifo (%s) !\n",
						__func__, pth.Path());
					continue;
				}

				LBKAppIPC *ipc = new LBKAppIPC;
				ipc->path.SetTo(pth.Path());
				ipc->fd = fd;
				fIPC = (void*)ipc;
#else
				// TODO: other way
#endif
			}

			// TODO: others
		}
	}
}


LBApplication::~LBApplication()
{
	for(int32 k = 0; k < CountPanels(); k++)
	{
		LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, k);

		LBView *view;
		while((view = (LBView*)dev->leftPageViews->RemoveItem(0)) != NULL) delete view;
		while((view = (LBView*)dev->rightPageViews->RemoveItem(0)) != NULL) delete view;
	}

	if(fPipes[0] >= 0)
	{
		close(fPipes[0]);
		close(fPipes[1]);
	}

	for(int32 k = 0; k < fAddOnsList.CountItems(); k++)
	{
		LBAddOnData *data = (LBAddOnData*)fAddOnsList.ItemAt(k);
		if(data->type == LBK_ADD_ON_PANEL_DEVICE)
			lbk_app_unload_panel_device_addon(data);
		// TODO: other addons
		unload_add_on(data->image);
		delete data;
	}

	if(fIPC != NULL)
	{
		LBKAppIPC *ipc = (LBKAppIPC*)fIPC;

#ifdef LBK_APP_IPC_BY_FIFO
		unlink(ipc->path.String());
		close(ipc->fd);
#else
		// TODO: other way
#endif

		delete ipc;
	}
}


bool
LBApplication::AddPageView(LBView *view, bool left_side, int32 panel_index)
{
	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, panel_index);
	if(dev == NULL || dev->valid == false || view == NULL) return false;
	if(view->fDev != NULL || view->Looper() != NULL || view->MasterView() != NULL) return false;

	if((left_side ? dev->leftPageViews->AddItem(view) : dev->rightPageViews->AddItem(view)) == false) return false;
	AddHandler(view);

	view->fActivated = false;
	view->fDev = PanelAt(panel_index);

	view->Attached();

	return true;
}


bool
LBApplication::RemovePageView(LBView *view)
{
	if(view == NULL || view->fDev == NULL || view->Looper() != this || view->MasterView() != NULL) return false;

	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, view->fDev->fID);

	view->Detached();
	RemoveHandler(view);

	if(dev->leftPageViews->RemoveItem(view) == false)
		dev->rightPageViews->RemoveItem(view);

	if(view == dev->preferHandler)
		dev->preferHandler = NULL;

	view->fActivated = false;
	view->fDev = NULL;

	return true;
}


LBView*
LBApplication::RemovePageView(int32 index, bool left_side, int32 panel_index)
{
	LBView *view = PageViewAt(index, left_side, panel_index);

	return(RemovePageView(view) ? view : NULL);
}


LBView*
LBApplication::PageViewAt(int32 index, bool left_side, int32 panel_index) const
{
	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, panel_index);

	if(dev == NULL) return NULL;
	return(left_side ? (LBView*)dev->leftPageViews->ItemAt(index) : (LBView*)dev->rightPageViews->ItemAt(index));
}


int32
LBApplication::CountPageViews(bool left_side, int32 panel_index) const
{
	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, panel_index);

	if(dev == NULL) return 0;
	return(left_side ? dev->leftPageViews->CountItems() : dev->rightPageViews->CountItems());
}


void
LBApplication::ActivatePageView(int32 index, bool left_side, int32 panel_index)
{
	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, panel_index);
	if(dev == NULL || dev->valid == false) return;

	LBView *newView = PageViewAt(index, left_side, panel_index);
	LBView *oldView = GetActivatedPageView(panel_index);

	if(oldView == newView || newView == NULL) return;

	if(oldView)
		oldView->SetActivated(false);

	dev->preferHandler = newView;
	newView->SetActivated(true);
}


LBView*
LBApplication::GetActivatedPageView(int32 panel_index) const
{
	LBPanelDeviceAddOnData *dev = lbk_app_get_panel_device_data(fAddOnsList, panel_index);

	if(dev == NULL) return NULL;
	// ignore NULL to make it compatible with old BeOS API
	return(dev->preferHandler ? cast_as(dev->preferHandler, LBView) : NULL);
}


bool
LBApplication::QuitRequested()
{
	fQuitLooper = true;

	if(fPipes[1] >= 0)
	{
		uint8 byte = 0xff;
		while(write(fPipes[1], &byte, 1) != 1) snooze(100000);
	}

	// The looper will be deleted automatically if this return "true".
	return false;
}


void
LBApplication::Go()
{
	if(IsRunning())
	{
		printf("[LBApplication]: It's forbidden to run Go() more than ONE time !\n");
		return;
	}

	if(pipe(fPipes) < 0)
	{
		perror("[LBApplication]: Unable to create pipe");
		return;
	}

	Lock();
	Run();
	for(int32 k = 0; k < CountPanels(); k++)
	{
		LBPanelDevice* dev = PanelAt(k);

		// WARNING: DO NOT CHANGE THE SEQUENCE OF FOLLOWING 2 LINES !!!
		dev->fMsgr = BMessenger(this, this);
		dev->fID = k;

		ActivatePageView(0, false, k);
	}
	LBKAppIPC *ipc = (LBKAppIPC*)fIPC;
	Unlock();

	fd_set rset;
	struct timeval timeout;
	uint32 count = 0;
	bigtime_t pulse_sent_time[2] = {0, 0};
	bigtime_t pulse_rate = 0;

	timeout.tv_sec = 0;
	while(IsRunning() && fQuitLooper == false)
	{
		timeout.tv_usec = (pulse_rate > 0 && pulse_rate < (bigtime_t)500000) ? pulse_rate : 500000;
		if(count > 0 && timeout.tv_usec > LBK_KEY_INTERVAL / 3)
			timeout.tv_usec = LBK_KEY_INTERVAL / 3;

		int fdMax = fPipes[0];
		FD_ZERO(&rset);
		FD_SET(fPipes[0], &rset);
		if(ipc != NULL)
		{
#ifdef LBK_APP_IPC_BY_FIFO
			FD_SET(ipc->fd, &rset);
			if(ipc->fd > fdMax) fdMax = ipc->fd;
#else
			// TODO: other way
#endif
		}
		int status = select(fdMax + 1, &rset, NULL, NULL, &timeout);
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
			PostMessage(B_PULSE, this);
			pulse_sent_time[0] = real_time_clock_usecs();
		}

		if(pulse_rate > 0 && real_time_clock_usecs() - pulse_sent_time[1] >= pulse_rate)
		{
			BMessage aMsg(B_PULSE);
			aMsg.AddBool("no_button_check", true);

			PostMessage(&aMsg, this);
			pulse_sent_time[1] = real_time_clock_usecs();
		}

		if(ipc != NULL)
		{
#ifdef LBK_APP_IPC_BY_FIFO
			if(status > 0 && FD_ISSET(ipc->fd, &rset))
			{
				uint8 byte = 0x00;
				while(true)
				{
					// avoid EWOULDBLOCK
					if(read(ipc->fd, &byte, 1) != 0) break;
				}
				switch(byte)
				{
					case 0xfe:
						PostMessage(LBK_APP_SETTINGS_UPDATED, this);
						break;

					default:
						// TODO
						break;
				}
			}
#else
			// TODO: other way
#endif
		}
	}

	PostMessage(B_QUIT_REQUESTED, this);
}


void
LBApplication::MessageReceived(BMessage *msg)
{
	int32 id = -1;
	LBPanelDeviceAddOnData *dev = NULL;

	bigtime_t when;
	uint8 key = 0;
	bool stopRunner;

	switch(msg->what)
	{
		case LBK_QUIT_REQUESTED:
			if(msg->FindInt32("panel_id", &id) == B_OK)
			{
				dev = lbk_app_get_panel_device_data(fAddOnsList, id);
				if(dev == NULL || dev->valid == false) break;

				LBView *view;
				while((view = (LBView*)dev->leftPageViews->RemoveItem(0)) != NULL) delete view;
				while((view = (LBView*)dev->rightPageViews->RemoveItem(0)) != NULL) delete view;

				dev->preferHandler = NULL;
				dev->valid = false;
			}
			else
			{
				PostMessage(B_QUIT_REQUESTED, this);
			}
			break;

		case B_KEY_DOWN:
		case B_KEY_UP:
			if(msg->FindInt32("panel_id", &id) != B_OK) break;
			if((dev = lbk_app_get_panel_device_data(fAddOnsList, id)) == NULL) break;
			if(dev->preferHandler == NULL) break;
			if(msg->FindInt8("key", (int8*)&key) != B_OK || key >= 8) break;
			if(msg->FindInt64("when", &when) != B_OK) break;
			if(msg->what == B_KEY_DOWN)
			{
				if((dev->keyState & (0x01 << key)) != 0) // already DOWN
				{
					// auto-repeat (event.value = 2) event
					if(when < dev->keyTimestamps[key]) break;
					if(when - dev->keyTimestamps[key] < (bigtime_t)600000) break; // 0.6s
					if(dev->keyClicks[key] == 0xff) break;
					dev->keyClicks[key] = 0xff; // long press
				}
				else
				{
					if(dev->keyClicks[key] > 0 && when < dev->keyTimestamps[key]) break;
					if(dev->keyClicks[key] < 0xff) dev->keyClicks[key]++;
				}
				dev->keyTimestamps[key] = when;

				BMessage aMsg(B_KEY_DOWN);
				aMsg.AddInt8("key", *((int8*)&key));
				aMsg.AddInt8("clicks", *((int8*)&dev->keyClicks[key]));
				aMsg.AddInt64("when", when);
				PostMessage(&aMsg, dev->preferHandler);

				dev->keyState |= (0x1 << key);
			}
			else
			{
				uint8 byte = 0xab;

				if((dev->keyState & (0x01 << key)) == 0) break; // already UP
				if(when < dev->keyTimestamps[key]) break;
				if(write(fPipes[1], &byte, 1) <= 0)
				{
					DBGOUT("[LBApplication]: Failed to notice the main thread.\n");
					BMessage aMsg(B_KEY_UP);
					aMsg.AddInt8("key", *((int8*)&key));
					aMsg.AddInt8("clicks", *((int8*)&dev->keyClicks[key]));
					aMsg.AddInt64("when", when);
					PostMessage(&aMsg, dev->preferHandler);

					dev->keyState &= ~(0x01 << key);
					dev->keyClicks[key] = 0;
					dev->keyTimestamps[key] = 0;
				}
				else
				{
					dev->keyState &= ~(0x01 << key);
					dev->keyTimestamps[key] = when;
				}
			}
			break;

		case B_PULSE:
			DBGOUT("[LBApplication]: B_PULSE received.\n");
			if(msg->HasBool("no_button_check"))
			{
				for(id = 0; id < CountPanels(); id++)
				{
					dev = lbk_app_get_panel_device_data(fAddOnsList, id);
					if(dev->preferHandler != NULL)
						PostMessage(B_PULSE, dev->preferHandler);
				}
				break;
			}

			stopRunner = true;
			when = real_time_clock_usecs();

			for(id = 0; id < CountPanels(); id++)
			{
				dev = lbk_app_get_panel_device_data(fAddOnsList, id);

				uint8 nKeys = CountPanelKeys(id);
				for(uint8 k = 0; k < nKeys; k++)
				{
					if((dev->keyState & (0x01 << k)) != 0) continue; // DOWN, no need
					if(dev->keyClicks[k] == 0 || dev->keyTimestamps[k] == 0) continue; // no UP before
					if(when < dev->keyTimestamps[k]) continue; // should never happen
					if(when - dev->keyTimestamps[k] < (bigtime_t)LBK_KEY_INTERVAL)
					{
						stopRunner = false;
						continue;
					}

					if(dev->preferHandler != NULL)
					{
						BMessage aMsg(B_KEY_UP);
						aMsg.AddInt8("key", *((int8*)&k));
						aMsg.AddInt8("clicks", *((int8*)&dev->keyClicks[k]));
						aMsg.AddInt64("when", dev->keyTimestamps[k]);
						PostMessage(&aMsg, dev->preferHandler);
					}

					dev->keyState &= ~(0x01 << k);
					dev->keyClicks[k] = 0;
					dev->keyTimestamps[k] = 0;
				}
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

		case LBK_APP_SETTINGS_UPDATED: // derived class should read the settings then pass it to LBApplication.
			if(msg->HasMessage("settings") == false) break;
			for(id = 0; id < CountPanels(); id++)
			{
				dev = lbk_app_get_panel_device_data(fAddOnsList, id);
				if(dev == NULL || dev->valid == false) continue;

				for(int32 k = 0; k < dev->leftPageViews->CountItems(); k++)
				{
					LBView *view = (LBView*)dev->leftPageViews->ItemAt(k);
					PostMessage(msg, view);
				}
				for(int32 k = 0; k < dev->rightPageViews->CountItems(); k++)
				{
					LBView *view = (LBView*)dev->rightPageViews->ItemAt(k);
					PostMessage(msg, view);
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


LBPanelDevice*
LBApplication::PanelAt(int32 index) const
{
	return lbk_app_get_panel_device(fAddOnsList, index);
}


uint8
LBApplication::CountPanelKeys(int32 index) const
{
	uint8 n = 0;

	LBPanelDevice *dev = PanelAt(index);
	if(dev != NULL)
		dev->GetCountOfKeys(n);

	return n;
}


int32
LBApplication::CountPanels() const
{
	return fPanelsCount;
}

