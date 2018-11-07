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


// TODO: label alignment, specified selection, max count, etc.
static void show_usage(void)
{
	printf("lbk-menu - Display menu on specified panel device.\n\n");
	printf("Usage: lbk-menu [options] label1 label2 [label3 ...]\n\
Options:\n\
    --conf config_path         Use the specified file as config file\n\
    -D device                  Index of panel device, default value is 0\n\
\n\
Return value:\n\
    Return index of label if user selected, 0 if user canceled, -1 if error occured.\n");
}


#define CMD_MSG_KEY		'cmdk'
#define CMD_MSG_CONFIRM		'cmdc'


static int32 cmd_ret = 0;
static LBApplication *cmd_app = NULL;


static filter_result cmd_msg_filter(BMessage *msg, BHandler **target, BMessageFilter *filter)
{
	uint8 key, clicks;
	int16 state;

	switch(msg->what)
	{
		case CMD_MSG_KEY:
			if(msg->FindInt8("key", (int8*)&key) != B_OK) break;
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(msg->FindInt16("down_state", &state) != B_OK) break;

			if(key == 1 && clicks == 0xff && (state & (0x0001 << 1)) != 0) // K2 long pressed
				cmd_app->PostMessage(B_QUIT_REQUESTED);
			break;

		case CMD_MSG_CONFIRM:
			if(msg->FindInt32("index", &cmd_ret) != B_OK)
				return B_DISPATCH_MESSAGE;
			cmd_ret += 1;
			cmd_app->PostMessage(B_QUIT_REQUESTED);
			break;

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

	int n;
	for(n = 1; n < argc; n++)
	{
		if(n < argc - 1 && strcmp(argv[n], "--conf") == 0)
		{
			path_conf.SetTo(argv[++n]);
		}
		else if(n < argc - 1 && strcmp(argv[n], "-D") == 0)
		{
			panel_index = atoi(argv[++n]);
		}
		else
		{
			break;
		}
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

	LBListView *listView = new LBListView(3);
	listView->MakeSelectable(true);
	listView->SetSelectionMessage(new BMessage('lbkm'));

	for(int k = 1; k < argc; k++)
		listView->AddItem(new LBListStringItem(argv[n + k]));

	if(cmd_app->AddPageView(listView, false, panel_index) == false)
	{
		fprintf(stderr, "No such panel device (id = %d) !\n", panel_index);
		delete listView;
	}
	else
	{
		if(selection >= 0)
			listView->SetPosition(selection);

		listView->SetMessage(new BMessage(CMD_MSG_CONFIRM));
		listView->SetKeyMessage(CMD_MSG_KEY);
		listView->SetTarget(BMessenger(listView));

		cmd_app->AddCommonFilter(new BMessageFilter(B_ANY_DELIVERY, B_LOCAL_SOURCE, cmd_msg_filter));

		cmd_app->Go();
	}

	cmd_app->Lock();
	cmd_app->Quit();

	return cmd_ret;
}

}; // extern "C"

