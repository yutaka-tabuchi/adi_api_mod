#include <stdio.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000L
#define KHZ 1000L


void SetDevinfo(adi_ad9081_device_t *ad9081_dev){
	int i;
    ad9081_dev->hal_info.sdo=SPI_SDO;
    ad9081_dev->hal_info.msb=SPI_MSB_FIRST;
    ad9081_dev->hal_info.addr_inc=SPI_ADDR_INC_AUTO;

    ad9081_dev->serdes_info.des_settings.boost_mask=0xff;
    ad9081_dev->serdes_info.des_settings.invert_mask=0x00;
    for(i=0;i<8;i++){
    ad9081_dev->serdes_info.des_settings.ctle_filter[i]=0;
    ad9081_dev->serdes_info.des_settings.lane_mapping[0][i]=i;
    ad9081_dev->serdes_info.des_settings.lane_mapping[1][i]=i;
    }
    ad9081_dev->serdes_info.ser_settings.invert_mask=0x00;
    for(i=0;i<8;i++){
    ad9081_dev->serdes_info.ser_settings.lane_mapping[0][i]=i;
    ad9081_dev->serdes_info.ser_settings.lane_mapping[1][i]=i;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].post_emp_setting=AD9081_SER_POST_EMP_3DB;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].pre_emp_setting=AD9081_SER_PRE_EMP_3DB;
    ad9081_dev->serdes_info.ser_settings.lane_settings[i].swing_setting=AD9081_SER_SWING_850;
    }

}

void ad9082_print_info(adi_ad9081_device_t *ad9081_dev)
{
    uint8_t reg_data;
    adi_ad9081_hal_reg_get(ad9081_dev, 0x3, &reg_data);
    printf("CHIP_TYPE     = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0x4, &reg_data);
    printf("PROD_ID_LSB   = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0x5, &reg_data);
    printf("PROD_ID_MSB   = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0x6, &reg_data);
    printf("CHIP_GRADE    = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0xb, &reg_data);
    printf("SPI_REVISION  = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0xc, &reg_data);
    printf("VENDER_ID_LSB = %02x\n", reg_data);
    adi_ad9081_hal_reg_get(ad9081_dev, 0xd, &reg_data);
    printf("VENDER_ID_MSB = %02x\n", reg_data);
}

int ad9082_chip()
{
    char *val;
    val = getenv("AD9082_CHIP");
    int chip = 0;
    if(val != NULL && strcmp(val, "1") == 0){
        chip = 1;
    }
    return chip;
}

int ad9082_adc_scramble()
{
    int flag = 1; // default enable
    char *val;
    val = getenv("AD9082_ADC_SCRAMBLE");
    if(val != NULL && strcmp(val, "0") == 0){
        flag = 0; // disabled if defiend and '0'
    }
    return flag;
}

void print_reg(adi_ad9081_device_t *ad9081_dev, int addr)
{
    uint8_t reg_data;
    adi_ad9081_hal_reg_get(ad9081_dev, addr, &reg_data);
    printf("0x%04X = 0x%02X\n", addr, reg_data);
}


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
    //printf("target addr:%s\n", target_addr);
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);
    //printf("%d>", step++); getchar();

    SetDevinfo(&ad9081_dev);
    adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
    printf("0x4=%X\n", reg_data);
    adi_ad9081_device_reset(&ad9081_dev, AD9081_SOFT_RESET);
    //printf("%d>", step++); getchar();
/*
    do{
      adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
      printf("0x4=%X\n", reg_data);
      sleep(1);
    }while(reg_data != 0x82);
*/

    adi_ad9081_device_init(&ad9081_dev);
    //printf("%d>", step++); getchar();
//return 0; // ok
#if 1
    adi_ad9081_device_clk_config_set(&ad9081_dev,dac_clk_hz, adc_clk_hz,dev_ref_clk_hz);
    adi_ad9081_hal_reg_get(&ad9081_dev, 0x4, &reg_data);
    printf("0x4=%X\n", reg_data);
//return 0; // dame
#endif
    //printf("%d>", step++); getchar();

    /******    DAC-Setup   ********/
    //uint8_t tx_main_interp = 4;
    //uint8_t tx_chan_interp = 6;
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
    //printf("%d>", step++); getchar();

    for(i=0;i<8;i++)
    	adi_ad9081_jesd_rx_lane_xbar_set(&ad9081_dev, AD9081_LINK_0,i, i);

    adi_ad9081_hal_reg_set(&ad9081_dev, 0x401,0x00);
    adi_ad9081_jesd_rx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);
    //printf("+%d>", step++); getchar();
    
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
    //printf("*%d>", step++); getchar();

    uint8_t lane[]={0,1,2,3,4,5,6,7};
    adi_ad9081_jesd_tx_lanes_xbar_set(&ad9081_dev, AD9081_LINK_0, lane);
    //printf("**%d>", step++); getchar();

    uint8_t lid[]={0,0,0,0,0,0,0,0};
    adi_ad9081_jesd_tx_lids_cfg_set(&ad9081_dev, AD9081_LINK_0, lid);
    //printf("***%d>", step++); getchar();
    adi_ad9081_jesd_tx_link_enable_set(&ad9081_dev, AD9081_LINK_0, 1);
    //printf("****%d>", step++); getchar();
    
    /******    GTH - reset   ********/
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x00);
    adi_ad9081_hal_reg_set(&ad9081_dev, 0x5BB, 0x01);
    //printf("%d>", step++); getchar();
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
