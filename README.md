# bsc-slave

Here is the source code of the BSC-SLAVE kernel module for 
the BCM2835 ARM soc used for the raspberry pi. It's been tested for 
the rasperry pi 2 and the B+.

You need to compile the module yourself with the included Makefile.
To do that you'll need to install the kernel sources.

#https://github.com/notro/rpi-source/wiki

When you're done compiling the module you can load it with:

#> insmod bsc-slave.ko
 
The 'i2ccat.c' file is a simple example of how to talk to the i2c controller 
of the pi with C. You give yourself a slave address and than read the FIFOs
by means of the read and write system call. You'll need to run the program
as root or make a udev rule to set the appropriate permissions automatically.

#SDA is on GPIO18 (PIN HEADER 12) while SCL is on GPIO19 (PIN HEADER 35)
