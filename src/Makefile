LIBRARIES = adi_ad9081_jesd.c \
            adi_ad9081_adc.c \
            axi_gpio_spi_i2c.c \
            axi_gpio.c \
            adi_ad9081_device.c \
            adi_ad9081_dac.c \
            adi_ad9081_hal.c \
            udpsendrecv.c \
            util.c

TARGETS = hello \
          gpio \
          spi_i2c \
          udpsendrecv \
          set_nco \
          set_xbar \
          set_fsc \
          set_dac_lane_mapping \
          get_jesd_status \
          print_ad9082_info \
          set_jesd_tx_link_conv_sel

all: ${TARGETS}


hello: ${LIBRARIES} helloworld.c
	gcc -O0 -o $@ $^

gpio: axi_gpio.c
	gcc -O0 -DAXI_GPIO_MAIN -o $@ $^

spi_i2c: axi_gpio.c axi_gpio_spi_i2c.c
	gcc -O0 -DAXI_GPIO_SPI_I2C_MAIN -o $@ $^

udpsendrecv: udpsendrecv.c
	gcc -O0 -DUDPSENDRECV_MAIN -o $@ $^

set_nco: ${LIBRARIES} set_nco.c
	gcc -O0 -o $@ $^

set_xbar: ${LIBRARIES} set_xbar.c
	gcc -O0 -o $@ $^

set_fsc: ${LIBRARIES} set_fsc.c
	gcc -O0 -o $@ $^

set_dac_lane_mapping: ${LIBRARIES} set_dac_lane_mapping.c
	gcc -O0 -o $@ $^

get_jesd_status: ${LIBRARIES} get_jesd_status.c
	gcc -O0 -o $@ $^

print_ad9082_info: ${LIBRARIES} print_ad9082_info.c
	gcc -O0 -o $@ $^

set_jesd_tx_link_conv_sel: ${LIBRARIES} set_jesd_tx_link_conv_sel.c
	gcc -O0 -o $@ $^


.PHONY: clean 

clean:
	-rm -f a.out
	-rm -f gpio
	-rm -f spi_i2c
	-rm -f hello
	-rm -f udpsendrecv
	-rm -f set_nco
	-rm -f set_xbar


