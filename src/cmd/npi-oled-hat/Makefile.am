INCLUDES = -I../../include -I../../libs/include

bin_PROGRAMS = npi_hat_cmd
noinst_PROGRAMS = oled_capture

npi_hat_cmd_SOURCES =	\
	npi_hat_cmd.c	\
	oled_clear.c	\
	oled_power.c	\
	oled_show.c	\
	oled_update.c
npi_hat_cmd_CFLAGS = $(INCLUDES) -DCMD_ALL_IN_ONE

oled_capture_SOURCES = oled_capture.c
oled_capture_CFLAGS = $(INCLUDES)

DISTCLEANFILES = Makefile.in

