#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))



uint8_t default_tx[] = { 0xFF, 0xAA, 0x66, 0xAA, 0xFF};

uint8_t default_rx[ARRAY_SIZE(default_tx)] = {0, };



static const char *device = "/dev/spidev3.0";
static uint32_t mode;
static uint8_t bits = 8;
static uint32_t speed = 5000;
static uint16_t delay;



static void hex_dump(unsigned char *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	unsigned char *address = src;
	unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			while (line < address) {
				c = *line++;
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			printf("\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}



static void transfer(int fd, uint8_t  *tx, uint8_t *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;


	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
{
	perror("can't send spi message");
	abort();
}

	/*if (verbose)
		hex_dump(tx, len, 32, "TX");

*/
	hex_dump(rx, len, 32, "RX");

}





int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint8_t *tx;
	uint8_t *rx;
	int size;

fd = open(device, O_RDWR);

if (fd < 0)
{
	perror("can't open device");
	abort();
}



	/*  bits per word */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
{
	perror("can't set bits per word");
	abort();
}
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
{
	perror("can't get bits per word");
	abort();
}



	/* max speed hz */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
{
	perror("can't set max speed hz");
	abort();
}
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
{
	perror("can't get max speed hz");
	abort();
}



        /* Display result */
	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);


/* transfer */
for(int kk=0;kk<40;kk++)
{
transfer(fd, default_tx, default_rx, sizeof(default_tx));
usleep(200000);
}

	close(fd);

	return ret;
}



