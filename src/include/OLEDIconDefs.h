/* --------------------------------------------------------------------------
 *
 * Panel Application for NanoPi OLED Hat
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
 * File: OLEDIconDefs.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __OLED_ICON_DEFS_H__
#define __OLED_ICON_DEFS_H__

#include <be/Be.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum {
	OLED_ICON_8x8 = 0,
	OLED_ICON_16x16,
	OLED_ICON_32x32,
};

typedef struct {
	uint8	type; 
	uint8	data[128];
} oled_icon;

typedef enum {
	OLED_ICON_OK = 0,
	OLED_ICON_YES,
	OLED_ICON_NO,
	OLED_ICON_HOME,
	OLED_ICON_UP,
	OLED_ICON_DOWN,
	OLED_ICON_LEFT,
	OLED_ICON_RIGHT,
	OLED_ICON_NONE = 0xff,
} oled_icon_id;


const oled_icon* oled_get_icon_data(oled_icon_id id_icon);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* __OLED_ICON_DEFS_H__ */

