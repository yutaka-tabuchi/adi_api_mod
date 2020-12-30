#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "axi_gpio.h"
#include "axi_gpio_spi_i2c.h"

#define BASEADDR (0xA0010000)

unsigned char axi_gpio_spi_read(unsigned int addr)
{
    struct axi_gpio_env_t env;
    if(axi_gpio_open(&env, BASEADDR) < 0){
        fprintf(stderr, "cannot access AXI GPIO\n");
        exit(1);
    }
    
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
    usleep(3000);
    axi_gpio_write(&env, 0, 0x0800); // [11]SPI1_CS = 1, [2]SPI0_CS = 0
    usleep(1000);
    
    addr = addr & 0x3FFF;
    addr = addr | 0x8000; // read mode
    
    int i;
    for(i = 0; i < 16; i++){
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0
        axi_gpio_write(&env, 0, 0x0800);
        usleep(2);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0, [0]MOSI
        axi_gpio_write(&env, 0, 0x0800 | ((addr & 0x8000) >> 15));
        usleep(10);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=1, [0]MOSI
        axi_gpio_write(&env, 0, 0x0802 | ((addr & 0x8000) >> 15));
        usleep(10);
        addr = addr << 1;
    }

    unsigned char ret = 0;
    for(i = 0; i < 8; i++){
        ret = ret << 1;
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0
        axi_gpio_write(&env, 0, 0x0800);
        usleep(10);
        int v = axi_gpio_read(&env, 8); // read from 2nd-channel
        ret = ret + (v & 0x00000001); // [0] SPI0_MISO
        // [2]SPI0_CS=0, [1]SPI0_SCLK=1
        axi_gpio_write(&env, 0, 0x0802);
        usleep(10);
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
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

    axi_gpio_write(&env, 0, 0x0804); // [2]SPI0_CS = 1
    usleep(3000);
    //CS=0
    axi_gpio_write(&env, 0, 0x0800); // [2]SPI0_CS = 1
    usleep(100);
    
    addr = addr & 0x3FFF;
    
    int i;
    for(i = 0; i < 16; i++){
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0
        axi_gpio_write(&env, 0, 0x0800);
        usleep(2);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0, [0]MOSI
        axi_gpio_write(&env, 0, 0x0800 | ((addr & 0x8000) >> 15));
        usleep(10);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=1, [0]MOSI
        axi_gpio_write(&env, 0, 0x0802 | ((addr & 0x8000) >> 15));
        usleep(10);
        addr = addr << 1;
    }
    
    for(i = 0; i < 8; i++){
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0
        axi_gpio_write(&env, 0, 0x0800);
        usleep(2);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=0, [0]MOSI
        axi_gpio_write(&env, 0, 0x0800 | ((data & 0x80)>>7));
        usleep(10);
        // [2]SPI0_CS=0, [1]SPI0_SCLK=1, [0]MOSI
        axi_gpio_write(&env, 0, 0x0802 | ((data & 0x80)>>7));
        usleep(10);
        data = data << 1;
    }

    usleep(100);
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
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
    
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
    usleep(3000);
    axi_gpio_write(&env, 0, 0x0004); // [11]SPI1_CS = 0, [2]SPI0_CS = 1
    usleep(100);
    
    addr = addr & 0x1FFF;
    addr = addr | 0x8000;
    
    int i;
    for(i = 0; i < 16; i++){
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0
        axi_gpio_write(&env, 0, 0x0004);
        usleep(2);
        int v = (addr & 0x8000);
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x004 | (v << 8));
        usleep(10);
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 1, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x404 | (v << 8)); // [11]SPI1_CS = 0, [8]SPI1_DO
        usleep(10);
        addr = addr << 1;
    }

    unsigned char ret = 0;
    for(i = 0; i < 8; i++){
        ret = ret << 1;
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0, [9]SPI1_DT = 1
        axi_gpio_write(&env, 0, 0x0204);
        usleep(10);
        int v = axi_gpio_read(&env, 8);
        ret = ret + ((v & 0x0000100) >> 8);
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 1, [9]SPI1_DT = 1
        axi_gpio_write(&env, 0, 0x0604);
        usleep(10);
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
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
    
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
    usleep(3000);
    axi_gpio_write(&env, 0, 0x0004); // [11]SPI1_CS = 0, [2]SPI0_CS = 1
    usleep(100);
    addr = addr & 0x1FFF;
    
    int i;
    for(i = 0; i < 16; i++){
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0
        axi_gpio_write(&env, 0, 0x0004);
        usleep(2);
        int v = (addr & 0x8000) >> 15;
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x004 | (v << 8));
        usleep(10);
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 1, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x404 | (v << 8)); // [11]SPI1_CS = 0, [8]SPI1_DO
        usleep(10);
        addr = addr << 1;
    }

    for(i = 0; i < 8; i++){
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0
        axi_gpio_write(&env, 0, 0x0004);
        usleep(2);
        int v = (data & 0x80) >> 7;
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 0, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x004 | (v << 8));
        usleep(10);
        // [11]SPI1_CS = 0, [10]SPI1_SCLK = 1, [8]SPI1_DO
        axi_gpio_write(&env, 0, 0x404 | (v << 8)); // [11]SPI1_CS = 0, [8]SPI1_DO
        usleep(10);
        data = data << 1;
    }
    
    usleep(100);
    axi_gpio_write(&env, 0, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
    usleep(10000);

    axi_gpio_close(&env);
}
