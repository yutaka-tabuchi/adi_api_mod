all: hello gpio spi_i2c udpsendrecv set_nco set_xbar set_dac_lane_mapping get_jesd_status

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

set_xbar: set_xbar.c
	gcc -DSET_XBAR_MAIN -O0 -o set_xbar adi_ad9081_jesd.c set_xbar.c adi_ad9081_adc.c axi_gpio_spi_i2c.c udpsendrecv.c axi_gpio.c adi_ad9081_device.c adi_ad9081_dac.c adi_ad9081_hal.c

set_dac_lane_mapping: set_dac_lane_mapping.c
	gcc -DSET_XBAR_MAIN -O0 -o set_dac_lane_mapping adi_ad9081_jesd.c set_dac_lane_mapping.c adi_ad9081_adc.c axi_gpio_spi_i2c.c udpsendrecv.c axi_gpio.c adi_ad9081_device.c adi_ad9081_dac.c adi_ad9081_hal.c

get_jesd_status: get_jesd_status.c
	gcc -DGET_JESD_STATUS -O0 -o get_jesd_status adi_ad9081_jesd.c get_jesd_status.c adi_ad9081_adc.c axi_gpio_spi_i2c.c udpsendrecv.c axi_gpio.c adi_ad9081_device.c adi_ad9081_dac.c adi_ad9081_hal.c

.PHONY: clean 

clean:
	-rm -f a.out
	-rm -f gpio
	-rm -f spi_i2c
	-rm -f hello
	-rm -f udpsendrecv
	-rm -f set_nco
	-rm -f set_xbar


