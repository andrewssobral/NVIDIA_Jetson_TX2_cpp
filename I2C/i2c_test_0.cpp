#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
  int file;
  char filename[20] = "/dev/i2c-1";
  file = open(filename, O_RDWR);
  if (file < 0) {
    std::cout<<"error: cannot open filename\n";
    return -1;
  }

  int addr = 0x40; /* The I2C address */

  int opening_slave=ioctl(file, I2C_SLAVE_FORCE, addr);
  if (opening_slave < 0) {
    std::cout<<opening_slave;
    std::cout<<"\nerror: cannot open address\n";
    return -2;
  }


int value =1; 
i2c_smbus_write_byte_data(file, 0xFA, 0); //reg=0xFA  , data = 0
i2c_smbus_write_byte_data(file, 0xFB, 0); //reg=0xFB  , data = 0
i2c_smbus_write_byte_data(file, 0xFC, value & 0xFF); //reg=0xFC  , data = value & 0xFF
i2c_smbus_write_byte_data(file, 0xFD, value >> 8); //reg=0xFD  , data = value >> 8


return 0;

}
