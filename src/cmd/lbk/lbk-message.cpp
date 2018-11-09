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
 * File: lbk-message.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <lbk/LBKit.h>


static void show_usage(void)
{
	printf("lbk-message - Display message on specified panel device.\n\n");
	printf("Usage: lbk-message [options] message\n\
Options:\n\
    -D device                  Index of panel device, default value: 0\n\
    --conf config_path         Use the specified file as config file\n\
    --k1 icon                  Specify the icon for K1, default value: none\n\
    --k2 icon                  Specify the icon for K2, default value: yes\n\
    --k3 icon                  Specify the icon for K3, default value: no\n\
    --type alert_type          Specify the alert type, default value: info\n\
    --topic topic              Specify the topic showing at the top line\n\
    --align left/center/right  Specify the alignment of icons to match keys\n\
    --response-even-none       Return even the icon of key is \"none\"\n\
    --timeout seconds          Exit after specified seconds\n\
    --long-press down/up       Specify state when key long-pressed, default: up\n\
\n\
Icon:\n\
    none, ok, yes, no, exit\n\
\n\
Alert type:\n\
    empty, info, idea, warning, stop\n\
\n\
Return value:\n\
    Return index(1 for K1 and so on) of the key if user pressed it; return\n\
100+index if user pressed the key for a long period; 0 if the pressed key isn't\n\
\"K1/K2/K3\" when specified with \"--response-even-none\"; -1 if error occured;\n\
-2 if timeout.\n\
    Usually, the negative exit code in shell will be turned to positive, such as,\n\
255 for -1, 254 for -2.\n");
}


#define CMD_MSG_CONFIRM		'cmdc'


static int32 cmd_ret = 0;
static LBApplication *cmd_app = NULL;


class TAlertView : public LBAlertView {
public:
	TAlertView(const char *title,
		   const char *text,
		   lbk_icon_id btn3_icon,
		   lbk_icon_id btn2_icon,
		   lbk_icon_id btn1_icon,
		   alert_type type,
		   bool long_press_up);

	void		SetResponseEvenNone();
	void		SetTimeout(int32 seconds);

	virtual void	KeyDown(uint8 key, uint8 clicks);
	virtual void	KeyUp(uint8 key, uint8 clicks);
	virtual void	MessageReceived(BMessage *msg);
	virtual void	Attached();
	virtual void	Pulse();

private:
	bool fRespEvenNone;
	bool fLongPressUp;
	bigtime_t fTimestamp;
};


TAlertView::TAlertView(const char *title,
		       const char *text,
		       lbk_icon_id btn3_icon,
		       lbk_icon_id btn2_icon,
		       lbk_icon_id btn1_icon,
		       alert_type type,
		       bool long_press_up)
	: LBAlertView(title, text, btn3_icon, btn2_icon, btn1_icon, type),
	  fRespEvenNone(false),
	  fLongPressUp(long_press_up),
	  fTimestamp(0)
{
}


void
TAlertView::SetResponseEvenNone()
{
	fRespEvenNone = true;
}


void
TAlertView::Pulse()
{
	if(fTimestamp > 0 && real_time_clock_usecs() > fTimestamp)
	{
		cmd_ret = -2;
		cmd_app->PostMessage(B_QUIT_REQUESTED);
	}
}


void
TAlertView::SetTimeout(int32 seconds)
{
	fTimestamp = (seconds > 0 ?
			(real_time_clock_usecs() + (bigtime_t)1000000 * (bigtime_t)seconds) : 0);
}


void
TAlertView::Attached()
{
	SetInvoker(new BInvoker(new BMessage(CMD_MSG_CONFIRM), BMessenger(this)));
}


void
TAlertView::KeyDown(uint8 key, uint8 clicks)
{
	LBAlertView::KeyDown(key, clicks);

	int32 id = IndexOfButton(key);
	if(clicks == 0xff && fLongPressUp == false && (id >= 0 || fRespEvenNone))
	{
		lbk_icon_id icon = GetButtonIcon(id);
		if(icon == LBK_ICON_NONE && fRespEvenNone == false) return;

		cmd_ret = (icon == LBK_ICON_NONE) ? 100 : (id + 101);
		cmd_app->PostMessage(B_QUIT_REQUESTED);
	}
}


void
TAlertView::KeyUp(uint8 key, uint8 clicks)
{
	LBAlertView::KeyUp(key, clicks);

	lbk_icon_id icon = GetButtonIcon(IndexOfButton(key));
	if(fRespEvenNone && icon == LBK_ICON_NONE)
	{
		cmd_ret = (clicks == 0xff) ? 100 : 0;
		cmd_app->PostMessage(B_QUIT_REQUESTED);
	}
}


void
TAlertView::MessageReceived(BMessage *msg)
{
	uint8 clicks;

	switch(msg->what)
	{
		case CMD_MSG_CONFIRM:
			if(msg->FindInt32("which", &cmd_ret) != B_OK) break;
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			cmd_ret += (clicks == 0xff) ? 101 : 1;
			cmd_app->PostMessage(B_QUIT_REQUESTED);
			break;

		default:
			LBAlertView::MessageReceived(msg);
	}
}


static lbk_icon_id cmd_get_icon_id(const char *str)
{
	if(str == NULL || *str == 0) return LBK_ICON_NONE;

	if(strcmp(str, "ok") == 0) return LBK_ICON_OK;
	if(strcmp(str, "yes") == 0) return LBK_ICON_YES;
	if(strcmp(str, "no") == 0) return LBK_ICON_NO;
	if(strcmp(str, "exit") == 0) return LBK_ICON_SMALL_EXIT;

	return LBK_ICON_NONE;
}


static alert_type cmd_get_alert_type(const char *str)
{
	if(str == NULL || *str == 0) return B_EMPTY_ALERT;

	if(strcmp(str, "info") == 0) return B_INFO_ALERT;
	if(strcmp(str, "idea") == 0) return B_IDEA_ALERT;
	if(strcmp(str, "warning") == 0) return B_WARNING_ALERT;
	if(strcmp(str, "stop") == 0) return B_STOP_ALERT;

	return B_EMPTY_ALERT;
}


extern "C" {

#ifdef CMD_ALL_IN_ONE
int cmd_message(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	BPath path_conf;
	int32 panel_index = 0;

	lbk_icon_id icon1 = LBK_ICON_NONE;
	lbk_icon_id icon2 = LBK_ICON_YES;
	lbk_icon_id icon3 = LBK_ICON_NO;

	alert_type t = B_INFO_ALERT;
	alignment align = B_ALIGN_RIGHT;
	BString topic;
	BString text;
	bool respNone = false;
	bool long_press_up = true;
	int32 timeout = -1;

	int n;
	for(n = 1; n < argc; n++)
	{
		if(strcmp(argv[n], "--response-even-none") == 0)
		{
			respNone = true;
		}
		else if(n >= argc - 1)
		{
			break;
		}
		else if(strcmp(argv[n], "--conf") == 0)
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
		else if(strcmp(argv[n], "--k1") == 0)
		{
			icon1 = cmd_get_icon_id(argv[++n]);
		}
		else if(strcmp(argv[n], "--k2") == 0)
		{
			icon2 = cmd_get_icon_id(argv[++n]);
		}
		else if(strcmp(argv[n], "--k3") == 0)
		{
			icon3 = cmd_get_icon_id(argv[++n]);
		}
		else if(strcmp(argv[n], "--type") == 0)
		{
			t = cmd_get_alert_type(argv[++n]);
		}
		else if(strcmp(argv[n], "--topic") == 0)
		{
			topic.SetTo(argv[++n]);
		}
		else if(strcmp(argv[n], "--timeout") == 0)
		{
			timeout = atoi(argv[++n]);
		}
		else if(strcmp(argv[n], "--long-press") == 0)
		{
			n++;
			if(strcmp(argv[n], "down") == 0)
				long_press_up = false;
			else if(strcmp(argv[n], "up") != 0)
				break;
		}
		else break;
	}
	argc -= (--n);

	if(argc < 2)
	{
		show_usage();
		exit(-1);
	}

	for(int k = 1; k < argc; k++)
	{
		if(k > 1) text.Append(" ");
		text.Append(argv[n + k]);
	}
	text.ReplaceAll("\\n", "\n");

	LBAppSettings cfg;
	if(path_conf.Path() != NULL)
	{
		BFile f(path_conf.Path(), B_READ_ONLY);
		cfg.AddItems(&f);
	}

	cmd_app = new LBApplication(&cfg);
	cfg.MakeEmpty();

	TAlertView *alert = new TAlertView(topic.String(),
					   text.String(),
					   icon3, icon2, icon1, t, long_press_up);
	if(respNone)
		alert->SetResponseEvenNone();
	alert->SetButtonAlignment(align);

	if(cmd_app->AddPageView(alert, false, panel_index) == false)
	{
		fprintf(stderr, "No such panel device (id = %d) !\n", panel_index);
		delete alert;

		cmd_ret = -1;
	}
	else
	{
		if(timeout > 0)
		{
			alert->SetTimeout(timeout);
			cmd_app->SetPulseRate(timeout > 4 ? 1000000 : 200000);
		}

		cmd_app->Go();
	}

	cmd_app->Lock();
	cmd_app->Quit();

	return cmd_ret;
}

}; // extern "C"

