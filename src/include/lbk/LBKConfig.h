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
 * File: LBKConfig.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LBK_BUILD_CONFIG_H__
#define __LBK_BUILD_CONFIG_H__

#include <be/Be.h>

//#define LBK_ENABLE_MORE_FEATURES

#define LBK_APP_IPC_BY_FIFO

#define LBK_KEY_TYPICAL_NUMBER		3
#define LBK_KEY_ID_MAX			7

// TODO: Specified by config file
#define LBK_KEY_INTERVAL		125000	/* 125ms */


#ifndef ETK_MAJOR_VERSION // for BeOS
#define _EXPORT		__declspec(dllexport)
#endif

#endif /* __LBK_BUILD_CONFIG_H__ */

