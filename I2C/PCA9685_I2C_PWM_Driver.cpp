#include <iostream>
#include <cstdint>
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

#include <unistd.h>


int file;
char filename[20] = "/dev/i2c-1";
int addr = 0x40; /* The I2C address */
int opening_slave;

void set_pwm(uint8_t pin,uint16_t value)
{
uint8_t y;

if(pin<16) y= 6 + (pin*4);
else y=0xFA;
if (value>4095) value=4095;

i2c_smbus_write_byte_data(file, y, 0);
i2c_smbus_write_byte_data(file, (y+1), 0);
i2c_smbus_write_byte_data(file, (y+2), value & 0xFF);
i2c_smbus_write_byte_data(file, (y+3), value >> 8);
}

int main(int argc, char **argv)
{
  
  
  file = open(filename, O_RDWR);
  if (file < 0) {
    std::cout<<"error: cannot open filename\n";
    return -1;
  }

  opening_slave=ioctl(file, I2C_SLAVE_FORCE, addr);
  if (opening_slave < 0) {
    std::cout<<opening_slave;
    std::cout<<"\nerror: cannot open address\n";
    return -2;
  }


// set frequency
  float freq = 1000;
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  i2c_smbus_write_byte_data(file, 0, 0x10); // sleep
  i2c_smbus_write_byte_data(file, 0xFE, prescaleval); // multiplyer for PWM frequency
  i2c_smbus_write_byte_data(file, 0, 0x80); //restart
  i2c_smbus_write_byte_data(file, 1, 0x04); //totem pole (default)


int on_value =0;
int value =1; // max 4095
for(uint16_t k=1;k<21;k++)
{
set_pwm(16,200*k);
std::cout<<"\n"<<(200*k);
usleep(1000000);
}
std::cout<<"\n";
return 0;

}
