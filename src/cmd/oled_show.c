#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char	bool;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#include "oled_ssd1306_ioctl.h"

void show_usage(void)
{
	printf("oled_show - show characters to OLED\n");
	printf("usage: oled_show device x y size string [erase_mode]\n\
    size = 12,14,16,24,32,0    0 means 32x32 icon (0-9)\n\
    erase_mode = 0,1           default value is 0\n");
}

int main(int argc, char **argv)
{
	int f, err = 0;
	_oled_ssd1306_show_t data;

	if(argc < 6 || argc > 7) {
		show_usage();
		exit(1);
	}

	if ((f = open(argv[1], O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	data.x = (uint8_t)atoi(argv[2]);
	data.y = (uint8_t)atoi(argv[3]);
	data.size = (uint8_t)atoi(argv[4]);
	strncpy(data.str, argv[5], sizeof(data.str));
	data.erase_mode = (argc < 7 || *argv[6] != '1') ? false : true;

	if ((err = ioctl(f, OLED_SSD1306_IOC_SHOW, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

