/* --------------------------------------------------------------------------
 *
 * Commands using Little Board Application Kit
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
 * File: lbk-menu.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <lbk/LBKit.h>


static void show_usage(void)
{
	printf("lbk-menu - Display menu on specified panel device.\n\n");
	printf("Usage: lbk-menu [options] label1 label2 [label3 ...]\n\
Options:\n\
    -D device                  Index of panel device, default value: 0\n\
    --conf config_path         Use the specified file as config file\n\
    --align left/center/right  Specify the alignment of lables, default value: left\n\
    --select index             Specify current selection's index, default value: 1\n\
    --max count                Maximum count to show on panel device at the same time\n\
    --timeout seconds          Exit after specified seconds\n\
\n\
Return value:\n\
    Return index(start from 1) of label if user selected; return 0 if user canceled\n\
by pressing \"OK\" for a long period; -1 if error occured; -2 if timeout.\n\
    Usually, the negative exit code in shell will be turned to positive, such as,\n\
255 for -1, 254 for -2.\n");
}


#define CMD_MSG_KEY		'cmdk'
#define CMD_MSG_CONFIRM		'cmdc'


static int32 cmd_ret = 0;
static LBApplication *cmd_app = NULL;
bigtime_t cmd_end_time = 0;


static filter_result cmd_msg_filter(BMessage *msg, BHandler **target, BMessageFilter *filter)
{
	LBView *view;
	void *source = NULL;
	uint8 key, clicks;
	int16 state;

	switch(msg->what)
	{
		case CMD_MSG_KEY:
			if(msg->FindInt8("key", (int8*)&key) != B_OK) break;
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(msg->FindInt16("down_state", &state) != B_OK) break;

			if(msg->FindPointer("source", &source) != B_OK) break;
			if(source == NULL || (view = reinterpret_cast<LBView*>(source)) == NULL) break;

			if(!is_kind_of(view, LBListView)) break;
			if(cast_as(view, LBListView)->GetNavButtonIcon((int32)key) != LBK_ICON_OK) break;

			if(clicks == 0xff && (state & (0x0001 << key)) == 0)
			{
				/*
				 * NOTE:
				 * 	Cancel the operation when "K2" long pressed then released.
				 * 	Double clicks will cause issue sometimes when pressing switch quickly.
				 */
				cmd_app->PostMessage(B_QUIT_REQUESTED);
			}
			break;

		case CMD_MSG_CONFIRM:
			if(msg->FindInt32("index", &cmd_ret) != B_OK) break;
			cmd_ret += 1;
			cmd_app->PostMessage(B_QUIT_REQUESTED);
			break;

		case B_PULSE:
			if(cmd_end_time > 0 && real_time_clock_usecs() > cmd_end_time)
			{
				cmd_ret = -2;
				cmd_app->PostMessage(B_QUIT_REQUESTED);
			}
			return B_DISPATCH_MESSAGE;

		default:
			return B_DISPATCH_MESSAGE;
	}

	return B_SKIP_MESSAGE;
}


extern "C" {

#ifdef CMD_ALL_IN_ONE
int cmd_menu(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	BPath path_conf;
	int32 panel_index = 0;
	int32 selection = -1;
	int32 max_count = 3;
	alignment align = B_ALIGN_LEFT;
	int32 timeout = -1;

	int n;
	for(n = 1; n < argc; n++)
	{
		if(n >= argc - 1) break;

		if(strcmp(argv[n], "--conf") == 0)
		{
			path_conf.SetTo(argv[++n]);
		}
		else if(strcmp(argv[n], "-D") == 0)
		{
			panel_index = atoi(argv[++n]);
		}
		else if(strcmp(argv[n], "--align") == 0)
		{
			n++;
			if(strcmp(argv[n], "right") == 0)
				align = B_ALIGN_RIGHT;
			else if(strcmp(argv[n], "center") == 0)
				align = B_ALIGN_CENTER;
			else if(strcmp(argv[n], "left") != 0)
				break;
		}
		else if(strcmp(argv[n], "--select") == 0)
		{
			selection = atoi(argv[++n]) - 1;
		}
		else if(strcmp(argv[n], "--max") == 0)
		{
			max_count = atoi(argv[++n]);
			if(max_count < 1) max_count = 1;
		}
		else if(strcmp(argv[n], "--timeout") == 0)
		{
			timeout = atoi(argv[++n]);
		}
		else break;
	}
	argc -= (--n);

	if(argc < 3)
	{
		show_usage();
		exit(-1);
	}

	LBAppSettings cfg;
	if(path_conf.Path() != NULL)
	{
		BFile f(path_conf.Path(), B_READ_ONLY);
		cfg.AddItems(&f);
	}

	cmd_app = new LBApplication(&cfg);
	cfg.MakeEmpty();

	LBListView *listView = new LBListView(max_count);
	listView->MakeSelectable(true);

	for(int k = 1; k < argc; k++)
		listView->AddItem(new LBListStringItem(argv[n + k]));

	if(cmd_app->AddPageView(listView, false, panel_index) == false)
	{
		fprintf(stderr, "No such panel device (id = %d) !\n", panel_index);
		delete listView;

		cmd_ret = -1;
	}
	else
	{
		if(selection >= 0)
			listView->SetPosition(selection);
		listView->SetItemsAlignment(align);

		listView->SetMessage(new BMessage(CMD_MSG_CONFIRM));
		listView->SetKeyMessage(CMD_MSG_KEY);
		listView->SetTarget(BMessenger(listView));

		cmd_app->AddCommonFilter(new BMessageFilter(B_ANY_DELIVERY, B_LOCAL_SOURCE, cmd_msg_filter));

		if(timeout > 0)
		{
			cmd_end_time = real_time_clock_usecs() + (bigtime_t)1000000 * (bigtime_t)timeout;
			cmd_app->SetPulseRate(timeout > 4 ? 1000000 : 200000);
		}

		cmd_app->Go();
	}

	cmd_app->Lock();
	cmd_app->Quit();

	return cmd_ret;
}

}; // extern "C"

