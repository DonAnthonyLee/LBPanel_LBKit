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
 * File: LBIconDefs.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_ICON_DEFS_H__
#define __LBK_ICON_DEFS_H__

#include <lbk/LBKConfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum {
	LBK_ICON_8x8 = 0,
	LBK_ICON_16x16,
	LBK_ICON_32x32,
};

typedef struct {
	uint8	type; 
	uint8	data[128];
} lbk_icon;

typedef enum {
	/* 8x8 */
	LBK_ICON_ID_8x8_BEGIN = 0x00,
	LBK_ICON_FOLDER,
	LBK_ICON_PLAIN_FILE,
	LBK_ICON_UNKNOWN_FILE,
	LBK_ICON_SQUARE,
	LBK_ICON_DOT,
	LBK_ICON_SMALL_LEFT,
	LBK_ICON_SMALL_RIGHT,
	LBK_ICON_ID_8x8_END,

	/* 16x16 */
	LBK_ICON_ID_16x16_BEGIN = 0x40,
	LBK_ICON_OK,
	LBK_ICON_YES,
	LBK_ICON_NO,
	LBK_ICON_HOME,
	LBK_ICON_UP,
	LBK_ICON_DOWN,
	LBK_ICON_LEFT,
	LBK_ICON_RIGHT,
	LBK_ICON_DOUBLE_DOT,
	LBK_ICON_SMALL_EXIT,
	LBK_ICON_ID_16x16_END,

	/* 32x32 */
	LBK_ICON_ID_32x32_BEGIN = 0x80,
	LBK_ICON_WARNING,
	LBK_ICON_REBOOT,
	LBK_ICON_POWER_OFF,
	LBK_ICON_SCREEN,
	LBK_ICON_INFO,
	LBK_ICON_STOP,
	LBK_ICON_IDEA,
	LBK_ICON_ABOUT,
	LBK_ICON_EXIT,
	LBK_ICON_SYSTEM,
	LBK_ICON_CUSTOM,
	LBK_ICON_ID_32x32_END,

	LBK_ICON_NONE = 0xff,
} lbk_icon_id;


_EXPORT const lbk_icon* lbk_get_icon_data(lbk_icon_id id_icon);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* __LBK_ICON_DEFS_H__ */

