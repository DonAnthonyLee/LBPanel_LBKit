/* --------------------------------------------------------------------------
 *
 * Defination for LCD ST7735S SPI driver
 * Copyright (C) 2018, Anthony Lee, All Rights Reserved
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * File: lcd_st7735s_ioctl.h
 * Description:
 *
 * --------------------------------------------------------------------------*/

#ifndef __LCD_ST7735S_IOCTL_H__
#define __LCD_ST7735S_IOCTL_H__

enum {
	_LCD_ST7735S_TIMESTAMP	= 0,
	_LCD_ST7735S_GET_PROP,
	_LCD_ST7735S_CLEAR,
	_LCD_ST7735S_DRAW_STRING,
	_LCD_ST7735S_UPDATE,
	_LCD_ST7735S_POWER,
	_LCD_ST7735S_STRING_WIDTH,
	_LCD_ST7735S_BUFFER,
	_LCD_ST7735S_INVERT,
	_LCD_ST7735S_SET_HIGH_COLOR,
	_LCD_ST7735S_SET_LOW_COLOR,
	_LCD_ST7735S_GET_PIXEL,
	_LCD_ST7735S_SET_CLIPPING,
	_LCD_ST7735S_SET_OFF_TIMEOUT,
};

typedef struct {
	int last_action;
	uint64_t ts;
} _lcd_st7735s_ts_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint8_t patterns[8];
	uint64_t ts;
} _lcd_st7735s_clear_t;

typedef struct {
	int16_t x;
	int16_t y;
	uint16_t color;
	char str[128];
	uint8_t reserved[5];
	uint8_t size;
	uint64_t ts;
} _lcd_st7735s_draw_str_t;

typedef struct {
	uint16_t w;
	uint16_t h;
	char str[128];
	uint8_t reserved[5];
	uint8_t size;
} _lcd_st7735s_string_width_t;

typedef struct {
	int state; // 0 and 1 for controling, others for accessing
	uint64_t ts;
} _lcd_st7735s_power_t;

typedef struct {
	uint16_t w;
	uint16_t h;
	int swap_xy;
} _lcd_st7735s_prop_t;

typedef struct {
	int action; // 0: recopy, 1: flush
	uint64_t ts;
} _lcd_st7735s_buffer_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint64_t ts;
} _lcd_st7735s_invert_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t color;
} _lcd_st7735s_pixel_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint64_t ts;
} _lcd_st7735s_set_clipping_t;

#define LCD_ST7735S_IOC_BASE			0x35

#define LCD_ST7735S_IOC_TIMESTAMP		_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_TIMESTAMP, _lcd_st7735s_ts_t)
#define LCD_ST7735S_IOC_GET_PROP		_IOR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_GET_PROP, _lcd_st7735s_prop_t)
#define LCD_ST7735S_IOC_CLEAR			_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_CLEAR, _lcd_st7735s_clear_t)
#define LCD_ST7735S_IOC_DRAW_STRING		_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_DRAW_STRING, _lcd_st7735s_draw_str_t)
#define LCD_ST7735S_IOC_UPDATE			_IOW(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_UPDATE, bool)
#define LCD_ST7735S_IOC_POWER			_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_POWER, _lcd_st7735s_power_t)
#define LCD_ST7735S_IOC_STRING_WIDTH		_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_STRING_WIDTH, _lcd_st7735s_string_width_t)
#define LCD_ST7735S_IOC_BUFFER			_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_BUFFER, _lcd_st7735s_buffer_t)
#define LCD_ST7735S_IOC_INVERT			_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_INVERT, _lcd_st7735s_invert_t)
#define LCD_ST7735S_IOC_SET_HIGH_COLOR		_IOW(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_SET_HIGH_COLOR, uint16_t)
#define LCD_ST7735S_IOC_SET_LOW_COLOR		_IOW(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_SET_LOW_COLOR, uint16_t)
#define LCD_ST7735S_IOC_GET_PIXEL		_IOR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_GET_PIXEL, _lcd_st7735s_pixel_t)
#define LCD_ST7735S_IOC_SET_CLIPPING		_IOWR(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_SET_CLIPPING, _lcd_st7735s_set_clipping_t)
#define LCD_ST7735S_IOC_SET_OFF_TIMEOUT		_IOW(LCD_ST7735S_IOC_BASE, _LCD_ST7735S_SET_OFF_TIMEOUT, uint64_t)

#endif /* __LCD_ST7735S_IOCTL_H__ */

