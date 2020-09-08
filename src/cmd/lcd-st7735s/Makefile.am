INCLUDES = -I../../include -I../../libs/include

bin_PROGRAMS = lcd_cmd

lcd_cmd_SOURCES =	\
	lcd_cmd.c	\
	lcd_clear.c	\
	lcd_power.c	\
	lcd_show.c	\
	lcd_update.c	\
	lcd_probe.c
lcd_cmd_CFLAGS = $(INCLUDES) -DCMD_ALL_IN_ONE

DISTCLEANFILES = Makefile.in

