/* --------------------------------------------------------------------------
 *
 * Commands using Little Board Application Kit
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
 * File: lbk-notify.c
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <lbk/LBKConfig.h>

#ifdef LBK_APP_IPC_BY_FIFO
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <fcntl.h>
	#include <unistd.h>
#else
	#error "No implementation for this command yet!"
#endif // !LBK_APP_IPC_BY_FIFO


void show_usage(void)
{
	printf("lbk-notify - Notify the application using LBKit.\n\n");
	printf("usage: lbk-notify ipc_name message_type\n\
    ipc_name                   Name of IPC.\n\
    mesage_type                Type of message\n\
        Valid value:\n\
            SETTINGS_UPDATED   Notify the applicaiton that the settings have been updated.\n");
}

int main(int argc, char **argv)
{
	int fd;
	char buf[1024];
	unsigned char cmd;

	if (argc != 3) {
		show_usage();
		exit(1);
	}

#ifdef LBK_APP_IPC_BY_FIFO
	if (strcmp(argv[2], "SETTINGS_UPDATED") == 0) {
		cmd = 0xfe;
	} else {
		show_usage();
		exit(1);
	}

	snprintf(buf, sizeof(buf), "/tmp/lbk_ipc_%u/%s", getuid(), argv[1]);

	if ((fd = open(buf, O_NONBLOCK | O_WRONLY)) < 0) {
		perror("Open");
		exit(1);
	}

	if (write(fd, &cmd, 1) != 1) {
		close(fd);
		perror("Write");
		exit(1);
	}

	close(fd);
#else
	// TODO: other way
#endif

	return 0;
}

