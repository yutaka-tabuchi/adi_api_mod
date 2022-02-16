#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"

#include "udpsendrecv.h"
#include "util.h"

#define KHZ 1000L
#define MHZ 1000000L

#define RX_ENABLE
//#define LANE_1

void ad9082_setup(adi_ad9081_device_t *ad9081_dev)
{

    uint64_t dac_clk_hz =     12000000000;
    uint64_t adc_clk_hz =     6000000000;
    uint64_t dev_ref_clk_hz = 12000000000;

    int i;
    uint8_t reg_data;
    int ret;

    // DEVICE INITIALIZATION AND CLOCK CONFIGURATION
    uint8_t rev_major, rev_minor, rev_rc;
    adi_ad9081_device_api_revision_get(ad9081_dev, &rev_major, &rev_minor, &rev_rc);
    printf("API Rev: %d.%d.%d\n", rev_major, rev_minor, rev_rc);
    
    ret = adi_ad9081_device_reset(ad9081_dev, AD9081_SOFT_RESET);
    if(ret < 0){ printf("reset error\n"); return; }
    ret = adi_ad9081_device_init(ad9081_dev);
    if(ret < 0){ printf("device init error\n"); return; }
    ret = adi_ad9081_device_clk_config_set(ad9081_dev, dac_clk_hz, adc_clk_hz, dev_ref_clk_hz);
    if(ret < 0){ printf("clk_config_set error\n"); return; }

    ad9082_print_info(ad9081_dev);

    adi_ad9081_hal_reg_set(ad9081_dev, 0x717, 0x01);
    adi_ad9081_hal_reg_set(ad9081_dev, 0x727, 0x50);

    // Tx CONFIGURATION
    uint8_t tx_main_interp = 6;
    uint8_t tx_chan_interp = 4;
    uint8_t tx_dac_chan[] = {0x1, 0x2, 0x1c, 0xe0};
    if(ad9082_chip() == 1){
        tx_dac_chan[0] = 0x07;
        tx_dac_chan[1] = 0x38;
        tx_dac_chan[2] = 0x40;
        tx_dac_chan[3] = 0x80;
    }
    int64_t tx_main_shift[] = {1000*MHZ, 1000*MHZ, 1000*MHZ, 1000*MHZ };
    int64_t tx_chan_shift[] = {0*MHZ, 0*MHZ, 0*MHZ, 0*MHZ, 0*MHZ, 0*MHZ, 0*MHZ, 0*MHZ };
#ifdef LANE_1	
    adi_cms_jesd_param_t jrx_param[2] =
        //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  V    Mode
        { { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   2},
          { 1, 4, 2, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   2} };
#else
    adi_cms_jesd_param_t jrx_param[2] =
        //  L  F   M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  V    Mode
        { { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   16},
          { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   16} };
#endif

    ret = adi_ad9081_device_startup_tx(ad9081_dev, tx_main_interp, tx_chan_interp,
                                 tx_dac_chan, tx_main_shift, tx_chan_shift, jrx_param);
    if(ret < 0){ printf("starup_tx error\n"); return; }

    for(i = 0; i < 8; i++){
       adi_ad9081_jesd_rx_lane_xbar_set(ad9081_dev, AD9081_LINK_0, i, i);
    }
    uint16_t tx_gains[] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
    ret = adi_ad9081_dac_duc_nco_gains_set(ad9081_dev, tx_gains);
    if(ret < 0){ printf("dac_duc_nco_gains_set error\n"); return; }
    //adi_ad9081_dac_mode_set(ad9081_dev, AD9081_DAC_MODE_SWITCH_GROUP_ALL, AD9081_DAC_MODE_0);
    //adi_ad9081_dac_xbar_set(ad9081_dev, AD9081_DAC_0, 0x01);
    //adi_ad9081_dac_xbar_set(ad9081_dev, AD9081_DAC_1, 0x01);
    //adi_ad9081_dac_xbar_set(ad9081_dev, AD9081_DAC_2, 0x01);
    //adi_ad9081_dac_xbar_set(ad9081_dev, AD9081_DAC_3, 0x01);
    //adi_ad9081_dac_fsc_set(ad9081_dev, AD9081_DAC_ALL, 26000);
    //adi_ad9081_dac_duc_nco_set(ad9081_dev, AD9081_DAC_ALL, AD9081_DAC_CH_ALL, 1000000000);

#ifdef RX_ENABLE
    // Rx CONFIGURATION
    uint8_t rx_cddc_select = AD9081_ADC_CDDC_ALL;
    uint8_t rx_fddc_select = AD9081_ADC_FDDC_ALL;
    int64_t cdcc_shift[]={1000*MHZ,1000*MHZ,1000*MHZ,1000*MHZ};
    int64_t fdcc_shift[]={0*KHZ,0*KHZ,0*KHZ,0*KHZ,0*KHZ,0*KHZ,0*KHZ,0*KHZ};
    uint8_t cddc_dcm[]={AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6,AD9081_CDDC_DCM_6};
    uint8_t fddc_dcm[]={AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,
	                AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2,AD9081_FDDC_DCM_2};
    uint8_t rx_cddc_c2r[]={0x0,0x0,0x0,0x0};
    uint8_t rx_fddc_c2r[]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

    adi_cms_jesd_param_t jtx_param[2] =
        //  L  F  M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R S
        { { 8, 4, 16, 1, 0, 64,16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   17, 0,  0 },
	  { 8, 4, 16, 1, 0, 64,16, 16, 0, 0, 0,  0, 0,   0,  1, 0,    2,   17, 0,  0 } };
    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
    if(ad9082_adc_scramble() == 0){
        jtx_param[0].jesd_scr = 0;
        jtx_param[1].jesd_scr = 0;
    }

    //printf("0>"); getchar();
    ret = adi_ad9081_device_startup_rx(ad9081_dev,
                                 rx_cddc_select,
                                 rx_fddc_select,
                                 cdcc_shift,
                                 fdcc_shift,
                                 cddc_dcm,
                                 fddc_dcm,
                                 rx_cddc_c2r,
                                 rx_fddc_c2r,
                                 jtx_param,
                                 jesd_conv_sel);
    if(ret < 0){ printf("startup_rx error\n"); return; }
    adi_ad9081_hal_reg_set(ad9081_dev, 0x0ca, 0x06);
    //printf("1>"); getchar();
    //adi_ad9081_adc_ddc_coarse_nco_mode_set(ad9081_dev, AD9081_ADC_CDDC_ALL, AD9081_ADC_NCO_VIF);
    //printf("2>"); getchar();
    uint8_t adc_cddc_xbar, cddc_fddc_xbar;
    adc_cddc_xbar = AD9081_ADC_2_ADC_COMP_MODE;
    cddc_fddc_xbar = AD9081_ADC_CDDC0_TO_FDDC0 | AD9081_ADC_CDDC1_TO_FDDC1 |
                     AD9081_ADC_CDDC0_TO_FDDC2 | AD9081_ADC_CDDC1_TO_FDDC3;
    cddc_fddc_xbar |= AD9081_ADC_CDDC2_TO_FDDC4 |
                      AD9081_ADC_CDDC3_TO_FDDC5 |
                      AD9081_ADC_CDDC2_TO_FDDC6 | AD9081_ADC_CDDC3_TO_FDDC7;
    ret = adi_ad9081_adc_xbar_set(ad9081_dev, adc_cddc_xbar, cddc_fddc_xbar);
    if(ret < 0){ printf("adc_xbar_set error\n"); return; }
    //printf("3>"); getchar();
    //ret = adi_ad9081_jesd_tx_fbw_sel_set(ad9081_dev, AD9081_LINK_0, 0xFFFF);
    //if(ret < 0){ printf("adi_ad9081_jesd_tx_fbw_sel_set error\n"); return; }
    //printf("4>"); getchar();
    //adi_ad9081_adc_nyquist_zone_set(ad9081_dev, AD9081_ADC_NYQUIST_ZONE_ODD);
    //printf("5>"); getchar();
    //ret = adi_ad9081_adc_ddc_fine_gain_set(ad9081_dev, AD9081_ADC_FDDC_0, 0);
    //if(ret < 0){ printf("adc_ddc_fine_gain_set error\n"); return; }
    //printf("6>"); getchar();

    //adi_ad9081_adc_ddc_coarse_nco_set(ad9081_dev, AD9081_ADC_CDDC_ALL, 2000*MHZ);
    //ret = adi_ad9081_adc_ddc_coarse_nco_enable_set(ad9081_dev, AD9081_ADC_CDDC_ALL);
    //if(ret < 0){ printf("ddc_coarse_nco_enable_set error\n"); return; }
    //adi_ad9081_adc_ddc_coarse_sync_enable_set(ad9081_dev, AD9081_ADC_CDDC_ALL, 0);

    //adi_ad9081_adc_ddc_fine_nco_set(ad9081_dev, AD9081_ADC_FDDC_ALL, 0*MHZ);
    //adi_ad9081_adc_ddc_fine_nco_enable_set(ad9081_dev, AD9081_ADC_FDDC_ALL);
    //uint8_t lane[]={0,1,2,3,4,5,6,7};
    //ret = adi_ad9081_jesd_tx_lanes_xbar_set(ad9081_dev, AD9081_LINK_0, lane);
    //if(ret < 0){ printf("tx_lane_xbar_set error\n"); return; }

#endif

    // LINK ESTABLISHMENT AND MONITORING
    ret = adi_ad9081_adc_nco_master_slave_sync(ad9081_dev, 1, 1, 0, 0);
    if(ret < 0){ printf("nco_master_slave_sync error\n"); return; }
    //printf("7>"); getchar();

#ifdef RX_ENABLE
    uint8_t lid[]={0,0,0,0,0,0,0,0};
    ret = adi_ad9081_jesd_tx_lids_cfg_set(ad9081_dev, AD9081_LINK_0, lid);
    if(ret < 0){ printf("jesd_tx_lids_cfg_set error\n"); return; }
    ret = adi_ad9081_jesd_tx_link_enable_set(ad9081_dev, AD9081_LINK_0, 1);
    if(ret < 0){ printf("jesd_tx_link_enable_set error\n"); return; }
#endif

    //printf("8>"); getchar();
    ret = adi_ad9081_jesd_rx_link_enable_set(ad9081_dev, AD9081_LINK_0, 1);
    if(ret < 0){ printf("jesd_rx_link_enable_set error\n"); return; }
    //printf("9>"); getchar();
    ret = adi_ad9081_jesd_rx_calibrate_204c(ad9081_dev, 1, 0x00, 0);
    if(ret < 0){ printf("jesd_rx_calibrate_204c error\n"); return; }
    //printf("10>"); getchar();
    ret = adi_ad9081_jesd_rx_link_enable_set(ad9081_dev, AD9081_LINK_0, 1);
    if(ret < 0){ printf("jesd_rx_link_enable error\n"); return; }
    //printf("11>"); getchar();

    adi_ad9081_hal_reg_set(ad9081_dev, 0x5BB, 0x00);
    adi_ad9081_hal_reg_set(ad9081_dev, 0x5BB, 0x01);
    for(i=0;i<4;i++){
	print_reg(ad9081_dev, 0x55E);
	print_reg(ad9081_dev, 0x5BB);
	print_reg(ad9081_dev, 0x62E);
	usleep(1000000);
    }

    uint8_t pll_status;
    adi_ad9081_jesd_pll_lock_status_get(ad9081_dev, &pll_status);
    printf("pll_status=%02x\n", pll_status);

    for(int i = 0; i < 8; i ++){
        print_reg(ad9081_dev, 0x670 + i);
    }
    print_reg(ad9081_dev, 0x702);
    print_reg(ad9081_dev, 0x728);
    print_reg(ad9081_dev, 0x0ca);
}

int main()
{
    adi_ad9081_device_t ad9081_dev;

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    //printf("target addr:%s\n", target_addr);
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);

    SetDevinfo(&ad9081_dev);
    
    ad9082_setup(&ad9081_dev);

    ad9082_print_info(&ad9081_dev);

    close_socket(&ad9081_dev.udp_env_info);

    return 0;
}
