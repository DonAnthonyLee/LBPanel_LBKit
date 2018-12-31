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
 * File: TApplication.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__

#include <lbk/LBKit.h>

#ifdef __cplusplus /* Just for C++ */

typedef struct {
	char *title;
	char *command;
	char *args;
} t_menu_item;

class TApplication : public LBApplication {
public:
	TApplication(const LBAppSettings *settings);
	virtual ~TApplication();

	virtual void		MessageReceived(BMessage *msg);

	int32			CountCustomMenuItems() const;
	const t_menu_item*	CustomMenuItemAt(int32 index) const;

private:
	int32 fScreenOffTimeout;
	BList fCustomMenu;
	BPath fConfigPath;

	void EmptyCustomMenu();
	void LoadConfig(const LBAppSettings *cfg);
};

#endif /* __cplusplus */

#endif /* __T_APPLICATION_H__ */

