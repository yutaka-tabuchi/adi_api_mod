/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
//#include "axi_gpio.h"
//#include "axi_gpio_spi_i2c.h"

void HMC7044_W(unsigned short addr,unsigned char data)
{
//    axi_gpio_i2c_write(addr, data);
}

unsigned char HMC7044_R(unsigned short addr)
{
    //return axi_gpio_i2c_read(addr);
    return 0;
}

void HMC7044_setup(){
	unsigned char ret;

    printf("Reset HMC7044\n\r");
    HMC7044_W(0x0000,0x01);
    HMC7044_W(0x0000,0x00);
    printf("Reset done!\n\r");
    ret=HMC7044_R(0x0003);//0x37
    printf("0x0011=%X\n",ret);
    /*
      GTH_REF  OUT12
      AD9082_REF OUT2
      
      
      PFD1=  R=16 N=16
      PFD2= R=1 N=32  HighBand
      
      TCXO=100MHz CLKIN0
      VCXO=100MHz OSCIN
    */
    //OptinumValue
    HMC7044_W(0x009F ,0x4D);
    HMC7044_W(0x00A0 ,0xDF);
    HMC7044_W(0x00A5 ,0x06);
    HMC7044_W(0x00A8 ,0x06);
    HMC7044_W(0x00B0 ,0x04);
    //PLL2-Setup
    HMC7044_W(0x0003 ,0x2F);
    HMC7044_W(0x0033 ,0x01);
    HMC7044_W(0x0035 ,0x20);
    //HMC7044_W(0x0037 ,0x04);  //CP-current
    //PLL1-Setup
    HMC7044_W(0x0014 ,0x00);
    HMC7044_W(0x001C ,0x01);
    HMC7044_W(0x0020 ,0x01);
    HMC7044_W(0x0021 ,0x10);
    HMC7044_W(0x0026 ,0x10);
    HMC7044_W(0x0029 ,0x00);
    //HMC7044_W(0x001A ,0x01);  //CP-current
    //Output2-Setup
    HMC7044_W(0x00DD ,0x01);
    //Output8-Setup
    HMC7044_W(0x0119 ,0x08);
    //Restart
    HMC7044_W(0x0001 ,0x02);
    HMC7044_W(0x0001 ,0x00);
}


void SetDevinfo(adi_ad9081_device_t *ad9081_dev){
    int i;
    ad9081_dev->hal_info.sdo=SPI_SDO;
    ad9081_dev->hal_info.msb=SPI_MSB_FIRST;
    ad9081_dev->hal_info.addr_inc=SPI_ADDR_INC_AUTO;
    
    ad9081_dev->serdes_info.des_settings.boost_mask=0x0;
    ad9081_dev->serdes_info.des_settings.invert_mask=0x0;
    for(i=0;i<8;i++){
        ad9081_dev->serdes_info.des_settings.ctle_filter[i]=0;
        ad9081_dev->serdes_info.des_settings.lane_mapping[0][i]=i;
        ad9081_dev->serdes_info.des_settings.lane_mapping[1][i]=i;
    }
    ad9081_dev->serdes_info.ser_settings.invert_mask=0x00;
    for(i=0;i<8;i++){
        ad9081_dev->serdes_info.ser_settings.lane_mapping[0][i]=i;
        ad9081_dev->serdes_info.ser_settings.lane_mapping[1][i]=i;
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].post_emp_setting=AD9081_SER_POST_EMP_0DB;
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].pre_emp_setting=AD9081_SER_PRE_EMP_0DB;
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].swing_setting=AD9081_SER_SWING_500;
    }
}

void SYNCIN_set(unsigned int sync)
{
    //unsigned int* sync_gpio=(unsigned int*)0xA0001000;
    //*sync_gpio = sync;
    //axi_gpio_write_once(0xA0020000, sync);
}

int main()
{
    int i;
    adi_ad9081_device_t ad9081_dev;
    uint64_t dac_clk_hz=3200000000;
    uint64_t adc_clk_hz=3200000000;
    uint64_t dev_ref_clk_hz=3200000000;
    
    //axi_gpio_write_once(0xA0010000, 0x0804); // [11]SPI1_CS = 1, [2]SPI0_CS = 1
    
    //axi_gpio_write_once(0xA0000000, 0x0000);
    //usleep(1000);
    //axi_gpio_write_once(0xA0000000, 0x0001);
    //usleep(1000);
    
    HMC7044_setup();

    SetDevinfo(&ad9081_dev);
    adi_ad9081_device_reset(&ad9081_dev, AD9081_SOFT_RESET);
    adi_ad9081_device_init(&ad9081_dev);

    printf("CHIP_TYPE     = %02x\n", AD9082_R(0x0003));
    printf("CHIP_GRADE    = %02x\n", AD9082_R(0x0006));
    printf("SPI_REVISION  = %02x\n", AD9082_R(0x000b));
    printf("VENDER_ID_LSB = %02x\n", AD9082_R(0x000c));
    printf("VENDER_ID_MSB = %02x\n", AD9082_R(0x000d));

    adi_ad9081_device_clk_config_set(&ad9081_dev,dac_clk_hz, adc_clk_hz,dev_ref_clk_hz);

    uint8_t rx_cddc_select = AD9081_ADC_CDDC_ALL;
    uint8_t rx_fddc_select = AD9081_ADC_FDDC_ALL;
    int64_t cdcc_shift[]={0,250000000,500000000,750000000};
    int64_t fdcc_shift[]={0,100000000,0,100000000,0,100000000,0,100000000};
    uint8_t cddc_dcm[]={AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2};
    uint8_t fddc_dcm[]={AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,};
    uint8_t rx_cddc_c2r[]={0,0,0,0};
    uint8_t rx_fddc_c2r[]={0,0,0,0,0,0,0,0};

    adi_cms_jesd_param_t jtx_param[2] =
    //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
    { { 8, 2, 8, 1, 0, 32, 16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    1,   10, 0,  0 },
      { 8, 2, 8, 1, 0, 32, 16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    1,   10, 0,  0 } };
    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]={{0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7},{0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7}};

    adi_ad9081_device_startup_rx(&ad9081_dev, rx_cddc_select, rx_fddc_select,cdcc_shift,fdcc_shift,
    		cddc_dcm, fddc_dcm, rx_cddc_c2r, rx_fddc_c2r, jtx_param,jesd_conv_sel);

    uint8_t lane[]={0,1,2,3,4,5,6,7};
    adi_ad9081_jesd_tx_lanes_xbar_set(&ad9081_dev, AD9081_LINK_0, lane);
    uint8_t lid[]={0,0,0,0,0,0,0,0};
    adi_ad9081_jesd_tx_lids_cfg_set(&ad9081_dev, AD9081_LINK_0, lid);

    adi_ad9081_jesd_tx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);

	SYNCIN_set(0x10);//GTH-reset
	sleep(1);
	SYNCIN_set(0x00);//GTH-reset-release
	sleep(1);
    for(i=0;i<100;i++){
    	SYNCIN_set(0x0A);
    	usleep(100);
    	SYNCIN_set(0x05);
    	usleep(200);
    }

    //axi_gpio_write_once(0xA0000000, 0x0000);
    
    return 0;
}
