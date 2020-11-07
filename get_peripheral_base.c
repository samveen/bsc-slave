/*
 * Source: https://www.raspberrypi.org/documentation/hardware/raspberrypi/peripheral_addresses.md
 */
#include <stdio.h>
#include <bcm_host.h>

int main(int argc,char*argv[]) {
    printf("0x%x\n", bcm_host_get_peripheral_address());
    return 0;
}
