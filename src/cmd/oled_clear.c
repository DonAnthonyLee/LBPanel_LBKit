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
	printf("oled_clear - clear OLED\n");
	printf("usage: oled_clear device x y width height [erase_mode]\n\
    erase_mode = 0,1           default value is 1\n");
}

int main(int argc, char **argv)
{
	int f, err = 0;
	_oled_ssd1306_clear_t data;

	if (!(argc == 2 || argc == 3 || argc == 6 || argc == 7)) {
		show_usage();
		exit(1);
	}
	if (argc == 3) {
		if (!(*argv[2] == '0' || *argv[2] == '1')) {
			show_usage();
			exit(1);
		}
	}

	if ((f = open(argv[1], O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	memset(&data, 0, sizeof(data));
	if (argc <= 3) {
		data.x = data.y = 0;
		data.w = 128;
		data.h = 64;
		if(!(argc == 2 || *argv[2] == '1')) memset(data.patterns, (int)0xff, sizeof(data.patterns));
	} else {
		data.x = (uint8_t)atoi(argv[2]);
		data.y = (uint8_t)atoi(argv[3]);
		data.w = (uint8_t)atoi(argv[4]);
		data.h = (uint8_t)atoi(argv[5]);
		if(!(argc < 7 || *argv[6] == '1')) memset(data.patterns, (int)0xff, sizeof(data.patterns));
	}

	if ((err = ioctl(f, OLED_SSD1306_IOC_CLEAR, &data)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

