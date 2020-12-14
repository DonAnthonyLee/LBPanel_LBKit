/* --------------------------------------------------------------------------
 *
 * Sample codes for LBKit
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
 * File: helloworld.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBKit.h>


class TView : public LBView {
public:
	TView();
	virtual ~TView(); // just for the sample codes

	virtual void		Draw(BRect r);
	virtual void		KeyUp(uint8 key, uint8 clicks);
};


TView::TView()
	: LBView(NULL)
{
}


TView::~TView()
{
}


void
TView::Draw(BRect r)
{
	const char str[] = "Hello world.";

	BPoint pt;
	BRect rect = Bounds();

	SetFontSize(12);
	uint16 w = StringWidth(str);

	pt.x = rect.left + (rect.Width() - w) / 2.f;
	pt.y = rect.top + (rect.Height() - 11) / 2.f;

	DrawString(str, pt);
}


void
TView::KeyUp(uint8 key, uint8 clicks)
{
	StandBack();
}


static char module_desc[] = "Hello World";
static const lbk_icon *module_icons[1];

extern "C" _EXPORT LBView* instantiate_panel_module(const char *path, const char *opt, const char** desc, const lbk_icon*** icons, int *icons_count)
{
	*desc = module_desc;

	module_icons[0] = lbk_get_icon_data(LBK_ICON_CUSTOM);

	*icons = module_icons;
	*icons_count = sizeof(module_icons) / sizeof(module_icons[0]);

	return new TView();
}
