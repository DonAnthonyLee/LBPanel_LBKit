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
 * File: NPIHatCombiner.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/add-ons/LBPanelKeypadGeneric.h>

#include "Config.h"
#include "NPIHatCombiner.h"

extern "C" _EXPORT LBPanelDevice* instantiate_panel_device()
{
	return new NPIHat();
}


NPIHat::NPIHat()
	: LBPanelCombiner()
{
	LBPanelKeypadGeneric *keypad = new LBPanelKeypadGeneric(DEFAULT_INPUT_EVENT);
	if(keypad->InitCheck(NULL) != B_OK || AddKeypad(keypad) != B_OK)
	{
		delete keypad;
		return;
	}
	keypad->SetKeycode(0, OLED_KEY1_CODE);
	keypad->SetKeycode(1, OLED_KEY2_CODE);
	keypad->SetKeycode(2, OLED_KEY3_CODE);

	BString optScreen;
	optScreen << "dev=" << DEFAULT_OLED_DEVICE;
	AddScreen("/usr/lib/add-ons/lbk/oled-ssd1306.so",
		  B_ORIGIN,
		  optScreen.String());
}


NPIHat::~NPIHat()
{
	// TODO
}


status_t
NPIHat::InitCheck(const char *options)
{
	// TODO
	return LBPanelCombiner::InitCheck(options);
}

