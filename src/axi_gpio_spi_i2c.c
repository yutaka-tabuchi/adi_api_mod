#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "axi_gpio.h"
#include "axi_gpio_spi_i2c.h"

#define BASEADDR (0xA0010000)

#define SPI0_MOSI(x) ((x & 0x00000001) << 0) // [0]
#define SPI0_SCLK (0x00000002) // [1]
#define SPI0_CS   (0x00000004) // [2]
#define SPI0_MISO(x) ((x & 0x00000001) >> 0) // [0]

#define SPI1_DO(x) ((x & 0x00000001) << 8) // [8]
#define SPI1_DT    (0x00000200) // [9]
#define SPI1_SCLK  (0x00000400) // [10]
#define SPI1_CS    (0x00000800) // [11]
#define SPI1_DI(x) ((x & 0x00000100) >> 8) // [8]

unsigned char axi_gpio_spi_read(unsigned int addr)
{
    struct axi_gpio_env_t env;
    if(axi_gpio_open(&env, BASEADDR) < 0){
        fprintf(stderr, "cannot access AXI GPIO\n");
        exit(1);
    }
    
    axi_gpio_write(&env, 0, SPI0_CS | SPI1_CS);
    usleep(3000);
    axi_gpio_write(&env, 0, SPI1_CS);
    usleep(1000);
    
    addr = addr & 0x3FFF;
    addr = addr | 0x8000; // read mode
    
    int i;
    for(i = 0; i < 16; i++){
        int v = (addr & 0x8000) >> 15;
        axi_gpio_write(&env, 0, SPI1_CS);
        usleep(2);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_MOSI(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_SCLK | SPI0_MOSI(v));
        usleep(10);
        addr = addr << 1;
    }

    unsigned char ret = 0;
    for(i = 0; i < 8; i++){
        ret = ret << 1;
        axi_gpio_write(&env, 0, SPI1_CS);
        usleep(10);
        int v = axi_gpio_read(&env, 8); // read from 2nd-channel
        ret = ret + SPI0_MISO(v);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_SCLK);
        usleep(10);
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(300);
    
    axi_gpio_close(&env);

    return ret;
}

void axi_gpio_spi_write(unsigned int addr, unsigned char data)
{
    struct axi_gpio_env_t env;
    if(axi_gpio_open(&env, BASEADDR) < 0){
        fprintf(stderr, "cannot access AXI GPIO\n");
        exit(1);
    }

    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(3000);
    axi_gpio_write(&env, 0, SPI1_CS);
    usleep(100);
    
    addr = addr & 0x3FFF;
    
    int i;
    for(i = 0; i < 16; i++){
        int v = (addr & 0x8000) >> 15;
        axi_gpio_write(&env, 0, SPI1_CS);
        usleep(2);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_MOSI(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_SCLK | SPI0_MOSI(v));
        usleep(10);
        addr = addr << 1;
    }
    
    for(i = 0; i < 8; i++){
        int v = (data & 0x80) >> 7;
        axi_gpio_write(&env, 0, SPI1_CS);
        usleep(2);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_MOSI(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI1_CS | SPI0_SCLK | SPI0_MOSI(v));
        usleep(10);
        data = data << 1;
    }

    usleep(100);
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(300);
    
    axi_gpio_close(&env);
}

unsigned char axi_gpio_i2c_read(unsigned int addr)
{
    struct axi_gpio_env_t env;
    if(axi_gpio_open(&env, BASEADDR) < 0){
        fprintf(stderr, "cannot access AXI GPIO\n");
        exit(1);
    }
    
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(3000);
    axi_gpio_write(&env, 0, SPI0_CS);
    usleep(100);
    
    addr = addr & 0x1FFF;
    addr = addr | 0x8000;
    
    int i;
    for(i = 0; i < 16; i++){
        axi_gpio_write(&env, 0, SPI0_CS);
        usleep(2);
        int v = (addr & 0x8000);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_DO(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_SCLK | SPI1_DO(v));
        usleep(10);
        addr = addr << 1;
    }

    unsigned char ret = 0;
    for(i = 0; i < 8; i++){
        ret = ret << 1;
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_DT);
        usleep(10);
        int v = axi_gpio_read(&env, 8);
        ret = ret + SPI1_DI(v);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_SCLK | SPI1_DT);
        usleep(10);
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(10000);
    
    axi_gpio_close(&env);

    return ret;
}

void axi_gpio_i2c_write(unsigned int addr, unsigned char data)
{
    struct axi_gpio_env_t env;
    if(axi_gpio_open(&env, BASEADDR) < 0){
        fprintf(stderr, "cannot access AXI GPIO\n");
        exit(1);
    }
    
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(3000);
    axi_gpio_write(&env, 0, SPI0_CS);
    usleep(100);
    addr = addr & 0x1FFF;
    
    int i;
    for(i = 0; i < 16; i++){
        int v = (addr & 0x8000) >> 15;
        axi_gpio_write(&env, 0, SPI0_CS);
        usleep(2);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_DO(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_SCLK | SPI1_DO(v));
        usleep(10);
        addr = addr << 1;
    }

    for(i = 0; i < 8; i++){
        int v = (data & 0x80) >> 7;
        axi_gpio_write(&env, 0, SPI0_CS);
        usleep(2);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_DO(v));
        usleep(10);
        axi_gpio_write(&env, 0, SPI0_CS | SPI1_SCLK | SPI1_DO(v));
        usleep(10);
        data = data << 1;
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, SPI1_CS | SPI0_CS);
    usleep(10000);

    axi_gpio_close(&env);
}

#ifdef AXI_GPIO_SPI_I2C_MAIN
int main(int argc, char **argv)
{
    if(argc < 4){
        printf("usage: %s spi|i2c read|write addr [data]\n", argv[0]);
        return 1;
    }
    
    int addr = strtoul(argv[3], NULL, 16);
    int data = 0;
    if(strcmp(argv[2], "write") == 0){
        if(argc < 5){
            printf("write command requires data\n");
            return 1;
        }
        data = strtoul(argv[4], NULL, 16);
    }
    
    if(strcmp(argv[1], "spi") == 0){
        if(strcmp(argv[2], "write") == 0){
            axi_gpio_spi_write(addr, data);
            printf("spi write: [%04x] <= [%02x]\n", addr, data);
        }else{
            int v = axi_gpio_spi_read(addr);
            printf("spi read: [%04x] => [%02x]\n", addr, v);
        }
    }else{
        if(strcmp(argv[2], "write") == 0){
            axi_gpio_i2c_write(addr, data);
            printf("i2c write: [%04x] <= [%02x]\n", addr, data);
        }else{
            int v = axi_gpio_i2c_read(addr);
            printf("i2c read: [%04x] => [%02x]\n", addr, v);
        }
    }
    
    return 0;
}
#endif
