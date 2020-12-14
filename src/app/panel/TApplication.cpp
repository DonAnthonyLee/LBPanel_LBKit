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

#include <stdio.h>

#include "TApplication.h"
#include "TMainPageView.h"
#include "TMenuPageView.h"
#include "TCommandsPageView.h"


TApplication::TApplication(const LBAppSettings *settings)
	: LBApplication(settings),
	  fScreenOffTimeout(-1)
{
	if(CountPanels() == 0) return;

	TCommandsPageView *cmdsView = new TCommandsPageView();
	AddPageView(cmdsView, true);
	AddPageView(new TMainPageView(), false);
	AddPageView(new TMenuPageView(), false);

	LoadConfig(settings);

	for(int32 k = 0; k < CountModules(); k++)
	{
		// TODO: find the proper icon for main panel
		const lbk_icon *icon = GetModuleIcon(k);

		cmdsView->AddModuleItem(GetModuleView(k), GetModuleDescription(k), icon);
	}
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
TApplication::LoadConfig(const LBAppSettings *cfg)
{
	bool use24Hours = false;
	bool showSeconds = false;
	int32 thermalZone = 0;
	int32 screenOffTimeout = fScreenOffTimeout;

	EmptyCustomMenu();

	for(int32 k = 0; k < cfg->CountItems(); k++)
	{
		BString name, value, options;

		if(cfg->GetItemAt(k, &name, &value, &options) != B_OK) continue;
		if(name.FindFirst("LBPanel::") != 0) continue;
		name.Remove(0, 9);

		if(name == "MenuItem" && options.Length() > 0) do
		{
			BString args;

			int32 found = options.FindFirst(",");
			if(found == 0) break;
			if(found > 0)
			{
				if(found < options.Length() - 1)
					args.SetTo(options.String() + found + 1);
				options.Truncate(found);
			}

			t_menu_item *item = (t_menu_item*)malloc(sizeof(t_menu_item));
			if(item == NULL || fCustomMenu.AddItem(item) == false)
			{
				if(item != NULL) free(item);
				break;
			}

			item->title = strdup(value.String());
			item->command = strdup(options.String());
			item->args = (args.Length() > 0) ? strdup(args.String()) : NULL;
		} while(false);

		if(name == "ScreenOffTimeout")
		{
			screenOffTimeout = (int32)atoi(value.String());
		}
		else if(name == "24Hours")
		{
			use24Hours = (value == "1" || value.ICompare("true") == 0);
		}
		else if(name == "ShowSeconds")
		{
			showSeconds = (value == "1" || value.ICompare("true") == 0);
		}
		else if(name == "ThermalZone")
		{
			thermalZone = (int32)atoi(value.String());
		}
		else if(name == "Config" && fConfigPath.Path() == NULL)
		{
			fConfigPath.SetTo(value.String(), NULL, true);
		}
	}

	TMainPageView *mainPageView = e_cast_as(PageViewAt(0, false), TMainPageView);
	if(mainPageView != NULL)
	{
		mainPageView->Set24Hours(use24Hours);
		mainPageView->ShowSeconds(showSeconds);
		mainPageView->SetThermalZone(thermalZone);
	}

	if(screenOffTimeout != fScreenOffTimeout)
	{
		fScreenOffTimeout = max_c(screenOffTimeout, 0);

		for(int32 k = 0; k < CountPanels(); k++)
		{
			LBPanelDevice *dev = PanelAt(k);
			if(dev != NULL)
				dev->SetPowerOffTimeout((bigtime_t)fScreenOffTimeout * (bigtime_t)1000000);
		}
	}
}


void
TApplication::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case LBK_APP_SETTINGS_UPDATED:
			if(fConfigPath.Path() != NULL)
			{
				LBAppSettings cfg;
				const LBAppSettings *old_cfg = Settings();
				for(int32 k = 0; k < old_cfg->CountItems(); k++)
				{
					BString name;

					if(old_cfg->GetItemAt(k, &name, NULL, NULL) != B_OK) continue;
					if(name.FindFirst("LBPanel::") == 0) continue;

					cfg.AddItem(old_cfg->ItemAt(k));
				}

				BFile f(fConfigPath.Path(), B_READ_ONLY);
				if(f.InitCheck() != B_OK || cfg.AddItems(&f) == false) break;
				LoadConfig(&cfg);

				// TODO: PostMessage() to LBApplication.
				SetSettings(&cfg);

				// Just for debug
				fprintf(stdout, "[TApplication]: Settings updated.\n");
			}
			break;

		default:
			LBApplication::MessageReceived(msg);
	}
};
