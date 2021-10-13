all: hello gpio spi_i2c udpsendrecv set_nco

hello: *.c
	gcc -O0 -o $@ $^

gpio: axi_gpio.c
	gcc -O0 -DAXI_GPIO_MAIN -o $@ $^

spi_i2c: axi_gpio.c axi_gpio_spi_i2c.c
	gcc -O0 -DAXI_GPIO_SPI_I2C_MAIN -o $@ $^

udpsendrecv: udpsendrecv.c
	gcc -O0 -DUDPSENDRECV_MAIN -o $@ $^

set_nco: set_nco.c
	gcc -DSET_NCO_MAIN -O0 -o set_nco adi_ad9081_jesd.c set_nco.c adi_ad9081_adc.c axi_gpio_spi_i2c.c udpsendrecv.c axi_gpio.c adi_ad9081_device.c adi_ad9081_dac.c adi_ad9081_hal.c

.PHONY: clean 

clean:
	-rm -f a.out
	-rm -f gpio
	-rm -f spi_i2c
	-rm -f hello
	-rm -f udpsendrecv
	-rm -f set_nco


