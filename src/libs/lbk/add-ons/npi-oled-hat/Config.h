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
 * File: Config.h
 * Description: Configuration for NanoPi OLED Hat
 *
 * --------------------------------------------------------------------------*/

#ifndef __NPI_OLED_HAT_CONFIG_H__
#define __NPI_OLED_HAT_CONFIG_H__

#define DEFAULT_OLED_DEVICE	"/dev/oled-003c"
#define DEFAULT_INPUT_EVENT	"/dev/input/event0"

#define OLED_SCREEN_WIDTH	128
#define OLED_SCREEN_HEIGHT	64

#define OLED_KEY_COUNT		3
#define OLED_KEY1_CODE		105	/* KEY_LEFT */
#define OLED_KEY2_CODE		102	/* KEY_HOME */
#define OLED_KEY3_CODE		106	/* KEY_RIGHT */

#endif /* __NPI_OLED_HAT_CONFIG_H__ */

