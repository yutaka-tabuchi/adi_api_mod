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
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000


void HMC7044_W(unsigned short addr,unsigned char data)
{

	
}

unsigned char HMC7044_R(unsigned short addr)
{

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
     * */
    //OptinumValue
    HMC7044_W(0x009F ,0x4D);
    HMC7044_W(0x00A0 ,0xDF);
    HMC7044_W(0x00A5 ,0x06);
    HMC7044_W(0x00A8 ,0x06);
    HMC7044_W(0x00B0 ,0x04);
    //PLL2-Setup
    HMC7044_W(0x0003 ,0x2F);
    HMC7044_W(0x0033 ,0x01);
    HMC7044_W(0x0035 ,0x20);  // N
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
    HMC7044_W(0x00DD ,0x01);  //DAC-CLK
    //Output8-Setup
    HMC7044_W(0x0119 ,0x08);  //450MHz
    //Restart
    HMC7044_W(0x0001 ,0x02);
    HMC7044_W(0x0001 ,0x00);
}


void SetDevinfo(adi_ad9081_device_t *ad9081_dev){
	int i;
    ad9081_dev->hal_info.sdo=SPI_SDO;
    ad9081_dev->hal_info.msb=SPI_MSB_FIRST;
    ad9081_dev->hal_info.addr_inc=SPI_ADDR_INC_AUTO;

    ad9081_dev->serdes_info.des_settings.boost_mask=0xFF;
    ad9081_dev->serdes_info.des_settings.invert_mask=0x00;
    for(i=0;i<8;i++){
    ad9081_dev->serdes_info.des_settings.ctle_filter[i]=0;
    ad9081_dev->serdes_info.des_settings.lane_mapping[0][i]=i;
    ad9081_dev->serdes_info.des_settings.lane_mapping[1][i]=i;
    }
    ad9081_dev->serdes_info.ser_settings.invert_mask=0xFF;
    for(i=0;i<8;i++){
    ad9081_dev->serdes_info.ser_settings.lane_mapping[0][i]=i;
    ad9081_dev->serdes_info.ser_settings.lane_mapping[1][i]=i;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].post_emp_setting=AD9081_SER_POST_EMP_0DB;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].pre_emp_setting=AD9081_SER_PRE_EMP_6DB;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].swing_setting=AD9081_SER_SWING_750;
    }

}


int main()
{

 
       uint8_t reg_data;
	int i;
	adi_ad9081_device_t ad9081_dev;
	uint64_t dac_clk_hz=3200000000;
	uint64_t adc_clk_hz=3200000000;
//	uint64_t dac_clk_hz=12000000000;
//	uint64_t adc_clk_hz= 3000000000;
	uint64_t dev_ref_clk_hz=3200000000;

    printf("Hello World \n\r");

    HMC7044_setup();

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    //printf("target addr:%s\n", target_addr);
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);

    SetDevinfo(&ad9081_dev);
    adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
    printf("0x4=%X\n", reg_data);
    adi_ad9081_device_reset(&ad9081_dev, AD9081_SOFT_RESET);
/*
    do{
      adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
      printf("0x4=%X\n", reg_data);
      sleep(1);
    }while(reg_data != 0x82);
*/

    adi_ad9081_device_init(&ad9081_dev);
//return 0; // ok
#if 1
    adi_ad9081_device_clk_config_set(&ad9081_dev,dac_clk_hz, adc_clk_hz,dev_ref_clk_hz);
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
        printf("0x4=%X\n", reg_data);
//return 0; // dame
#endif

    /******    DAC-Setup   ********/
   /* uint8_t tx_main_interp = 12;
    uint8_t tx_chan_interp =2;
    uint8_t tx_dac_chan[] = {0x01,0x01,0x01,0x01};
    int64_t tx_main_shift[] = {100*MHZ,100 * MHZ,100* MHZ,100 * MHZ };
    int64_t tx_chan_shift[] = {10 * MHZ,10 * MHZ,10 * MHZ,10 * MHZ,10 * MHZ,10 * MHZ,10 * MHZ,10 * MHZ };
    adi_cms_jesd_param_t jrx_param[2] =
        //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
    { { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    2,   2,   0,  0 },
      { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    2,   2,   0,  0 } };

    adi_ad9081_device_startup_tx(&ad9081_dev, tx_main_interp, tx_chan_interp,
    tx_dac_chan, tx_main_shift, tx_chan_shift, jrx_param);

    for(i=0;i<8;i++)
    	adi_ad9081_jesd_rx_lane_xbar_set(&ad9081_dev, AD9081_LINK_0,3, i);

    adi_ad9081_hal_reg_set(&ad9081_dev, 0x401,0xF7); //lane3=0xF7
    adi_ad9081_jesd_rx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);*/


    /******    ADC-Setup   ********/
    uint8_t rx_cddc_select = AD9081_ADC_CDDC_ALL;
    uint8_t rx_fddc_select = AD9081_ADC_FDDC_ALL;
    int64_t cdcc_shift[]={0,250000000,500000000,750000000};
    int64_t fdcc_shift[]={0,100000000,0,100000000,0,100000000,0,100000000};
   // uint8_t cddc_dcm[]={AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2,AD9081_CDDC_DCM_2};
    uint8_t cddc_dcm[] = { AD9081_CDDC_DCM_1,AD9081_CDDC_DCM_1,AD9081_CDDC_DCM_1,AD9081_CDDC_DCM_1 };
    uint8_t fddc_dcm[]={AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4,AD9081_FDDC_DCM_4};
    uint8_t rx_cddc_c2r[]={0,0,0,0};
    uint8_t rx_fddc_c2r[]={0,0,0,0,0,0,0,0};

    adi_cms_jesd_param_t jtx_param[2] =
    //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
    { { 8, 2, 8, 1, 0, 128,16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    2,   10, 0,  0 },
      { 8, 2, 8, 1, 0, 128,16, 16, 0, 0, 0,  0, 0,   0,  0, 0,    2,   10, 0,  0 } };
    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]={{0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7},{0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7}};

    adi_ad9081_device_startup_rx(&ad9081_dev, rx_cddc_select, rx_fddc_select,cdcc_shift,fdcc_shift,
    		cddc_dcm, fddc_dcm, rx_cddc_c2r, rx_fddc_c2r, jtx_param,jesd_conv_sel);
    		usleep(1000000);
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
        printf("0x4=%X\n", reg_data);
    		usleep(1000000);
    uint8_t lane[]={0,1,2,3,4,5,6,7};
    adi_ad9081_jesd_tx_lanes_xbar_set(&ad9081_dev, AD9081_LINK_0, lane);
    		usleep(1000000);
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
        printf("0x4=%X\n", reg_data);
    		usleep(1000000);
    uint8_t lid[]={0,0,0,0,0,0,0,0};
    adi_ad9081_jesd_tx_lids_cfg_set(&ad9081_dev, AD9081_LINK_0, lid);
    adi_ad9081_jesd_tx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);
    
    /******    GTH - reset   ********/
 
    for(i=0;i<4;i++){
        adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x00);
        adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x01);
    	adi_ad9081_hal_reg_get(&ad9081_dev,0x55E,&reg_data);
    	printf("0x55E=%X\n",reg_data);
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x5BB, &reg_data);
        printf("0x5BB=%X\n", reg_data);
        
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x62E, &reg_data);
        printf("0x62E=%X\n", reg_data);
    }


    return 0;
}
