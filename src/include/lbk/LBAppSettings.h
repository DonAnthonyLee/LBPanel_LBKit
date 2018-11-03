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
 * File: LBAppSettings.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_APPLICATION_SETTINGS_H__
#define __LBK_APPLICATION_SETTINGS_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus /* Just for C++ */

class _EXPORT LBAppSettings {
public:
	LBAppSettings();
	~LBAppSettings();

	bool			AddItem(const char *item, int32 index = -1);
	bool			AddItems(BFile *f, int32 index = -1);
	bool			AddItems(const LBAppSettings &settings, int32 index = -1);
	void			MakeEmpty();

	LBAppSettings&		operator+=(const LBAppSettings &settings);
	LBAppSettings&		operator=(const LBAppSettings &settings);

	const char*		operator[](int index) const;
	const char*		ItemAt(int32 index) const;
	int32			CountItems() const;

private:
	BList fSettings;
};

#endif /* __cplusplus */

#endif /* __LBK_APPLICATION_SETTINGS_H__ */

