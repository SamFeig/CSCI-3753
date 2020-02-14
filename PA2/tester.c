#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define DEVICE "/dev/pa2_character_device"
#define BUFFER_SIZE 1024

int main() {
	bool exit = false;
	char choice;
	int fd = open(DEVICE, O_RDWR);

	if(fd < 0) {
		printf("Unable to open %s", DEVICE);
		return -1;
	}

	while(!exit) {
		printf("\n");
		printf("Read('r'), Write('w'), Seek('s'), Exit('e')\n");
		printf("Enter what you would like to do:\n");
		scanf(" %c", &choice);
		getchar();

		switch(choice) {
			case 'r': {
				int readBytes;
				printf("Enter the number of bytes you want to read:\n");
				scanf("%d", &readBytes);

				printf("Data read from the device:\n");
				char *readBuf = malloc(readBytes);
				read(fd, readBuf, readBytes);

				for(int i = 0; i < readBytes; i++) {
					printf("%c", *(readBuf + i));
				}
				printf("\n");
				free(readBuf);
				break;
			}
			case 'w': {
				char buffer[BUFFER_SIZE];
				int status;
				printf("Enter data you want to write to the device:\n");
				fgets(buffer, BUFFER_SIZE, stdin);

				if(buffer[strlen(buffer) - 1] == '\n'){
					status = write(fd, buffer, strlen(buffer)-1);
				} else {
					status = write(fd, buffer, strlen(buffer));
				}
				if(status == 0) {
					printf("Data was written to the device\n");
				}
				else {
					printf("Failed to write to the device\n");
				}
				break;
			}
			case 's': {
				int offset, whence;
				printf("Enter an offset value:\n");
				scanf("%d", &offset);
				printf("Enter a value for whence (third parameter):\n");
				scanf("%d", &whence);

				lseek(fd, offset, whence);
				break;
			}
			case 'e':
				printf("Exiting.\n");
				exit = true;
				break;
			default:
				printf("Continuing...\n");
				break;
		}
	}

	close(fd);
	return 0;
}





