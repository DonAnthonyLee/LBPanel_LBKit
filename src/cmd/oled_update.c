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
	printf("oled_update - whether to update when OLED drawing\n");
	printf("usage: oled_update device [state]\n\
    state = 0,1           default value is 1\n");
}

int main(int argc, char **argv)
{
	int f, err = 0;
	bool state;

	if(argc < 2 || argc > 3) {
		show_usage();
		exit(1);
	}

	if ((f = open(argv[1], O_RDWR)) < 0) {
		perror("Open");
		exit(1);
	}

	state = (argc == 2 || *(argv[2]) == '1') ? true : false;

	if ((err = ioctl(f, OLED_SSD1306_IOC_UPDATE, &state)) != 0)
		perror("Ioctl");

	close(f);

	return err;
}

