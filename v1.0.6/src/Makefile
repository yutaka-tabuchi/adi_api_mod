LIBRARIES = adi_ad9081_adc.c \
            adi_ad9081_dac.c \
            adi_ad9081_device.c \
            adi_ad9081_hal.c \
            adi_ad9081_jesd.c \
            axi_gpio.c \
            axi_gpio_spi_i2c.c \
            udpsendrecv.c \
            util.c

all: hello gpio spi_i2c udpsendrecv

hello: ${LIBRARIES} helloworld.c
	gcc -o $@ $^

gpio: axi_gpio.c
	gcc -DAXI_GPIO_MAIN -o $@ $^

spi_i2c: axi_gpio.c axi_gpio_spi_i2c.c
	gcc -DAXI_GPIO_SPI_I2C_MAIN -o $@ $^

udpsendrecv: udpsendrecv.c
	gcc -DUDPSENDRECV_MAIN -o $@ $^

.PHONY: clean 

clean:
	-rm -f a.out
	-rm -f gpio
	-rm -f spi_i2c
	-rm -f hello
	-rm -f udpsendrecv
