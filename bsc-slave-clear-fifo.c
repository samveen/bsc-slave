#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <stdint.h>
#include "bsc-slave.h"


#define SLV_ADDR  	 0x33
#define TX_BUF_SIZE      5

int main(void){

	char buffer[20];
	char tx_buffer[TX_BUF_SIZE];
	int fd;
	int length, tx_length = 2;
	int count, value_count, transfered;
	char *pointer;
	int output_end = 0;

	if((fd = open("/dev/i2c-slave", O_RDWR)) == -1){
		printf("could not open i2c-slave\n");
	}

	ioctl(fd, I2C_SLAVE_CLEAR_FIFOS, 0);

	close(fd);
	return 0;
}

