INCLUDES = -I../../include -I../../libs/include

bin_PROGRAMS = oled_cmd
noinst_PROGRAMS = oled_capture

oled_cmd_SOURCES =	\
	oled_cmd.c	\
	oled_clear.c	\
	oled_power.c	\
	oled_show.c	\
	oled_update.c
oled_cmd_CFLAGS = $(INCLUDES) -DCMD_ALL_IN_ONE

oled_capture_SOURCES = oled_capture.c
oled_capture_CFLAGS = $(INCLUDES)

DISTCLEANFILES = Makefile.in

