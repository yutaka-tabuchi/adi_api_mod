#include <stdio.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000L
#define KHZ 1000L

#include "util.h"

int main()
{

    int step = 0;
 
    uint8_t reg_data;
    int i;
    adi_ad9081_device_t ad9081_dev;
    uint64_t dac_clk_hz=    11640000000;
    uint64_t adc_clk_hz=     2910000000*2;
    uint64_t dev_ref_clk_hz=11640000000;

    printf("Hello World \n\r");

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);

    SetDevinfo(&ad9081_dev);
    adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
    printf("0x4=%X\n", reg_data);
    adi_ad9081_device_reset(&ad9081_dev, AD9081_SOFT_RESET);

    adi_ad9081_device_init(&ad9081_dev);

    adi_ad9081_device_clk_config_set(&ad9081_dev,dac_clk_hz, adc_clk_hz,dev_ref_clk_hz);
    adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
    printf("0x4=%X\n", reg_data);

    /******    DAC-Setup   ********/
    uint8_t tx_main_interp = 6;
    uint8_t tx_chan_interp = 4;
    uint8_t tx_dac_chan[] = {0x01, 0x02, 0x1C, 0xE0};

    if(ad9082_chip() == 1){
        tx_dac_chan[0] = 0x07;
        tx_dac_chan[1] = 0x38;
        tx_dac_chan[2] = 0x40;
        tx_dac_chan[3] = 0x80;
    }

    //int64_t tx_main_shift[] = {2910L*MHZ, 2910L*MHZ, 2910L*MHZ, 2910L*MHZ }; // 3000MHz
    int64_t tx_main_shift[] = {2716L*MHZ, 2716L*MHZ, 2716L*MHZ, 2716L*MHZ }; // 2800MHz
    //int64_t tx_main_shift[] = {2328*MHZ, 2328*MHZ, 2328*MHZ, 2328*MHZ }; // 2400MHz
    //int64_t tx_main_shift[] = {2134L*MHZ, 2134L*MHZ, 2134L*MHZ, 2134L*MHZ }; // 2200MHz
    //int64_t tx_main_shift[] = {1940*MHZ, 1940*MHZ, 1940*MHZ, 1940*MHZ }; // 2000MHz
    //int64_t tx_main_shift[] = {1746*MHZ, 1746*MHZ, 1746*MHZ, 1746*MHZ }; // 1800MHz
    //int64_t tx_main_shift[] = {1649*MHZ, 1649*MHZ, 1649*MHZ, 1649*MHZ }; // 1700MHz
    //int64_t tx_main_shift[] = {1552*MHZ, 1552*MHZ, 1552*MHZ, 1552*MHZ }; // 1600MHz
    //int64_t tx_main_shift[] = {1455*MHZ, 1455*MHZ, 1455*MHZ, 1455*MHZ }; // 1500MHz
    //int64_t tx_main_shift[] = {1358*MHZ, 1358*MHZ, 1358*MHZ, 1358*MHZ }; // 1400MHz
    int64_t tx_chan_shift[] = {0*KHZ, 0*KHZ, 0*KHZ, 0*KHZ, 0*KHZ, 0*KHZ, 0*KHZ, 0*KHZ };
#ifdef USE_1LANE
    adi_cms_jesd_param_t jrx_param[2] =
        //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
        { { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   2,   0,  0 },
          { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   2,   0,  0 } };
#else
    adi_cms_jesd_param_t jrx_param[2] =
        //  L  F  M   S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
        { { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   16,   0,  0 },
          { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   16,   0,  0 } };
#endif

    adi_ad9081_device_startup_tx(&ad9081_dev, tx_main_interp, tx_chan_interp,
    tx_dac_chan, tx_main_shift, tx_chan_shift, jrx_param);

    for(i=0;i<8;i++)
    	adi_ad9081_jesd_rx_lane_xbar_set(&ad9081_dev, AD9081_LINK_0,i, i);

    adi_ad9081_hal_reg_set(&ad9081_dev, 0x401,0x00);
    adi_ad9081_jesd_rx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);
    
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x01B,0x0F);// morisaka-220118  DAC-mask
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x117,0xA0);// morisaka-220118  fullscale-current
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x118,0xFF);// morisaka-220118  fullscale-current

    /******    ADC-Setup   ********/
    uint8_t rx_cddc_select = AD9081_ADC_CDDC_ALL;
    uint8_t rx_fddc_select = AD9081_ADC_FDDC_ALL;
    int64_t cdcc_val, fdcc_val;
    cdcc_val = 1164000000;
    fdcc_val = 0;
    int64_t cdcc_shift[]={cdcc_val, cdcc_val, cdcc_val, cdcc_val};
    int64_t fdcc_shift[]={fdcc_val, fdcc_val, fdcc_val, fdcc_val, fdcc_val, fdcc_val, fdcc_val, fdcc_val}; 
    uint8_t cddc_dcm[] = { AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6};
    uint8_t fddc_dcm[]={AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2};
    uint8_t rx_cddc_c2r[]={0,0,0,0};
    uint8_t rx_fddc_c2r[]={0,0,0,0,0,0,0,0};

    adi_cms_jesd_param_t jtx_param[2] =
    //  L  F   M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R   S
    { { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0,  0,  0,  0,  0,  1,   0,   2, 17,   0,  0 },
      { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0,  0,  0,  0,  0,  1,   0,   2, 17,   0,  0 } };
    if(ad9082_adc_scramble() == 0){
        jtx_param[0].jesd_scr = 0;
        jtx_param[1].jesd_scr = 0;
    }
    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};

    adi_ad9081_device_startup_rx(&ad9081_dev, rx_cddc_select, rx_fddc_select,cdcc_shift,fdcc_shift,
    		cddc_dcm, fddc_dcm, rx_cddc_c2r, rx_fddc_c2r, jtx_param,jesd_conv_sel);

    uint8_t lane[]={0,1,2,3,4,5,6,7};
    adi_ad9081_jesd_tx_lanes_xbar_set(&ad9081_dev, AD9081_LINK_0, lane);

    uint8_t lid[]={0,0,0,0,0,0,0,0};
    adi_ad9081_jesd_tx_lids_cfg_set(&ad9081_dev, AD9081_LINK_0, lid);
    adi_ad9081_jesd_tx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);
    
    /******    GTH - reset   ********/
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x00);
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x01);

    for(i=0;i<8;i++){
    	adi_ad9081_hal_reg_get(&ad9081_dev,0x55E,&reg_data);
    	printf("0x55E=%X\n",reg_data);
        adi_ad9081_hal_reg_get(&ad9081_dev, 0x5BB, &reg_data);
        printf("0x5BB=%X\n", reg_data);
        usleep(100000);
    }
    for(int i = 0; i < 8; i++){
        print_reg(&ad9081_dev, 0x670+i);
    }
    print_reg(&ad9081_dev, 0x702);
    print_reg(&ad9081_dev, 0x728);
    print_reg(&ad9081_dev, 0x0ca);

    ad9082_print_info(&ad9081_dev);
    if(ad9082_adc_scramble() == 0){
      printf("ADC9082 ADC SCRAMBLE is disabled\n");
    }

    return 0;
}
