#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/ioctl.h>
#include "bsc-slave.h"
#include "rI2CTX.h"

#define SLV_ADDR  	 0x33

int main(void){
	char buffer[20];
	char tx_buffer[200];
	int fd;
	int tx_length = 2;
	int count, transfered;
	char *pointer;

	if ((fd = open("/dev/i2c-slave", O_RDWR)) == -1){
		printf("could not open i2c-slave\n");
	}

	if ((ioctl(fd, I2C_SLAVE, SLV_ADDR) < 0)){
		printf("failed setting slave adress!\n");
		return -1;
	}

	read(fd, buffer, 20);

	rI2CTX_beginFrame();
	rI2CTX_addParameter(0, (int8_t)-25);
	rI2CTX_addParameter(1, (uint8_t)25);
	rI2CTX_addParameter(2, (int16_t)-1000);
	rI2CTX_addParameter(3, (uint16_t)1000);
	rI2CTX_addParameter(4, (int64_t)-1000);
	rI2CTX_addParameter(5, (uint64_t)1000);
	rI2CTX_addParameter(6, (float)-250.252525);
	rI2CTX_addParameter(7, (double)-250.252525);
	rI2CTX_endFrame();

	for (count = 0; count < rI2CTX_bufferPos; count++){
		tx_buffer[count] = rI2CTX_buffer[count];
	}

	tx_length = rI2CTX_bufferPos;
	pointer = tx_buffer;
	while (tx_length > 0){             //send values
		transfered = write(fd, pointer, tx_length);
		for (count = 0; count < transfered; count++){
			printf("send value is %d\n", (int)*pointer++);
			tx_length--;
		}
	}

	fsync(fd);
	close(fd);
	return 0;
}
