#ifndef __AXI_GPIO_SPI_I2C_H__
#define __AXI_GPIO_SPI_I2C_H__

unsigned char axi_gpio_spi_read(unsigned int addr);
void axi_gpio_spi_write(unsigned int addr, unsigned char data);
unsigned char axi_gpio_i2c_read(unsigned int addr);
void axi_gpio_i2c_write(unsigned int addr, unsigned char data);

#endif /* __AXI_GPIO_SPI_I2C_H__ */
