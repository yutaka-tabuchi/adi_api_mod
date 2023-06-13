#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "util.h"

struct env {
    int disabled_messages;
    int both_mode;
    int adc_mode;
    int fine_mode;
    int ch;
    int adc_ch;
    int dac_ch;
    int64_t freq;
    int freq_valid;
};

void ad9082_coarse_nco_adda(adi_ad9081_device_t *dev, struct env *e)
{
    int32_t err;
    uint64_t ftw;
    err = adi_ad9081_hal_calc_tx_nco_ftw(dev, dev->dev_info.dac_freq_hz, e->freq, &ftw);
    err = adi_ad9081_dac_duc_nco_ftw_set(dev, AD9081_DAC_0 << e->dac_ch, AD9081_DAC_CH_NONE, ftw, 0, 0);

    int cddc = AD9081_ADC_CDDC_0 << e->adc_ch;
    err = adi_ad9081_adc_ddc_coarse_nco_mode_set(dev, cddc, (e->freq == 0) ? AD9081_ADC_NCO_ZIF : AD9081_ADC_NCO_VIF);
    //err = adi_ad9081_hal_calc_rx_nco_ftw(device, device->dev_info.adc_freq_hz, cddc_shift_hz, &ftw);
    ftw *= 2; // use double of DAC's ftw (it is assumed that sampling freq. of DAC and ADC are 12GHz and 6GHz, respectively)
    err = adi_ad9081_adc_ddc_coarse_nco_ftw_set(dev, cddc, ftw, 0, 0);
}

void ad9082_fine_nco_adda(adi_ad9081_device_t *dev, struct env *e)
{
    int32_t err;
    uint64_t ftw;
    uint8_t main_interp = 0;
    // DAC
    err = adi_ad9081_hal_bf_get(dev, REG_INTRP_MODE_ADDR, BF_FINE_INTERP_SEL_INFO, &main_interp, 1);
    err = adi_ad9081_dac_duc_nco_enable_set(dev, AD9081_DAC_NONE, AD9081_DAC_CH_0 << e->dac_ch, 1);
    err = adi_ad9081_hal_calc_tx_nco_ftw(dev, dev->dev_info.dac_freq_hz, e->freq * main_interp, &ftw);
    err = adi_ad9081_dac_duc_nco_ftw_set(dev, AD9081_DAC_NONE, AD9081_DAC_CH_0 << e->dac_ch, ftw, 0, 0);

    // ADC
    int fddc = AD9081_ADC_FDDC_0 << e->adc_ch;
    err = adi_ad9081_adc_ddc_fine_nco_mode_set(dev, fddc, (e->freq == 0) ? AD9081_ADC_NCO_ZIF : AD9081_ADC_NCO_VIF);
    uint64_t cddc_dcm_value = adi_ad9081_adc_ddc_coarse_dcm_decode(AD9081_CDDC_DCM_6); // SHOULD be same value defined in helloworld.c
    //adc_freq_hz = adc_freq_hz / cddc_dcm_value;
    //err = adi_ad9081_hal_calc_rx_nco_ftw(dev, adc_freq_hz, e->freq, &ftw);
    ftw *= 2; // use double of DAC's ftw (it is assumed that sampling freq. of DAC and ADC are 2GHz and 1GHz, respectively)
    err = adi_ad9081_adc_ddc_fine_nco_ftw_set(dev, fddc, ftw, 0, 0);
}


void ad9082_setup(adi_ad9081_device_t *dev, struct env *e)
{
    int32_t err;
    if(e->both_mode == 1){
        // both DAC and ADC
        if(e->fine_mode == 0){ // COARSE
            ad9082_coarse_nco_adda(dev, e);
        }else{
            ad9082_fine_nco_adda(dev, e);
        }
    }else if(e->adc_mode == 0){
        // DAC
        if(e->fine_mode == 0){ // COARSE
	  /* err = adi_ad9081_dac_duc_nco_set(dev, AD9081_DAC_0 << e->ch, AD9081_DAC_CH_NONE, e->freq);
           */
	  { /* TABUCHI SPECIAL, mask the lowest 16 bits */
	    uint64_t ftw;
            err = adi_ad9081_hal_calc_tx_nco_ftw(
                          dev, dev->dev_info.dac_freq_hz, e->freq, &ftw);
	    if( API_CMS_ERROR_OK == err )
              err = adi_ad9081_dac_duc_nco_ftw_set(
                          dev, AD9081_DAC_0 << e->ch, AD9081_DAC_CH_NONE, ftw & 0xffffffff0000, 0, 0);
	  }
        }else{ // FINE
	  /* err = adi_ad9081_dac_duc_nco_set(dev, AD9081_DAC_NONE, AD9081_DAC_CH_0 << e->ch, e->freq);
           */
          if(1){ /* TABUCHI SPECIAL, mask the lowest 18 bits */
            uint64_t ftw;
            uint8_t main_interp = 0;

            err = adi_ad9081_hal_bf_get(dev, REG_INTRP_MODE_ADDR, BF_FINE_INTERP_SEL_INFO, &main_interp, 1);
            if ( API_CMS_ERROR_OK == err )
              err = adi_ad9081_dac_duc_nco_enable_set(dev, AD9081_DAC_NONE,
						      AD9081_DAC_CH_0 << e->ch, 1);
            if ( API_CMS_ERROR_OK == err )
              err = adi_ad9081_hal_calc_tx_nco_ftw( dev, dev->dev_info.dac_freq_hz,
						    e->freq * main_interp, &ftw);
            if ( API_CMS_ERROR_OK == err )
              err = adi_ad9081_dac_duc_nco_ftw_set( dev, AD9081_DAC_NONE,
						    AD9081_DAC_CH_0 << e->ch,
						    ftw & 0xfffffffc0000, 0, 0 );
          }
        }
    }else{
        // ADC
        if(e->fine_mode == 0){ // COARSE
            int cddc = AD9081_ADC_CDDC_0 << e->ch;
            err = adi_ad9081_adc_ddc_coarse_nco_mode_set(dev, cddc,
                                                         (e->freq == 0) ? AD9081_ADC_NCO_ZIF : AD9081_ADC_NCO_VIF);
            /* err = adi_ad9081_adc_ddc_coarse_nco_set(dev, cddc, e->freq);
             */
	    { /* TABUCHI SPECIAL, mask the lowest 16 bits */
	      uint64_t ftw;
              err = adi_ad9081_hal_calc_rx_nco_ftw(
                dev, dev->dev_info.adc_freq_hz, e->freq, &ftw);
	      if ( API_CMS_ERROR_OK == err )
                err = adi_ad9081_adc_ddc_coarse_nco_ftw_set(dev, cddc, ftw & 0xffffffff0000, 0, 0);
	    }
        }else{ // FINE
            uint64_t adc_freq_hz, ftw;
            int fddc = AD9081_ADC_FDDC_0 << e->ch;
            err = adi_ad9081_adc_ddc_fine_nco_mode_set(dev, fddc,
                                                       (e->freq == 0) ? AD9081_ADC_NCO_ZIF : AD9081_ADC_NCO_VIF);
            adc_freq_hz = dev->dev_info.adc_freq_hz;
            uint64_t cddc_dcm_value = adi_ad9081_adc_ddc_coarse_dcm_decode(AD9081_CDDC_DCM_6); // SHOULD be same value defined in helloworld.c
            adc_freq_hz = adc_freq_hz / cddc_dcm_value;
            err = adi_ad9081_hal_calc_rx_nco_ftw(dev, adc_freq_hz, e->freq, &ftw);
            err = adi_ad9081_adc_ddc_fine_nco_ftw_set(dev, fddc, ftw, 0, 0);
        }
    }
}

void print_usage()
{
    printf("usage: set_nco\n");
    printf("\t--help           \tprint this message\n");
    printf("\t--freq           \tset frequency(> 0)\n");
    printf("\t--fine-mode      \tto set fine NCO(default; coarse NCO)\n");
    printf("\t--both-mode      \tset set both NCO of ADC and DAC (default; only ADC or DAC)\n");
    printf("\t--adc-channel    \tset ADC target channel for both-mode (between 0 and 7 for fine NCO, between 0 and 3 for corase NCO)\n");
    printf("\t--dac-channel    \tset DAC target channel for both-mode (between 0 and 7 for fine NCO, between 0 and 3 for corase NCO)\n");
    printf("\t--adc-mode       \tset ADC-side NCO (default; DAC-side NCO)\n");
    printf("\t--channel        \tset ADC or DAC target channel (between 0 and 7 for fine NCO, between 0 and 3 for corase NCO)\n");
    printf("\t--disable-message\tsuppress human readable message\n");
}

int validate_env(struct env *e)
{
    if(e->both_mode == 1){
        if(e->fine_mode == 1){
            if(e->adc_ch < 0 || e->adc_ch > 7){
                return 0;
            }
            if(e->dac_ch < 0 || e->dac_ch > 7){
                return 0;
            }
        }else{
            if(e->adc_ch < 0 || e->adc_ch > 3){
                return 0;
            }
            if(e->dac_ch < 0 || e->dac_ch > 3){
                return 0;
            }
        }
    }else{
        if(e->fine_mode == 0 && (e->ch < 0 || e->ch > 3)){
            return 0;
        }
        if(e->fine_mode == 1 && (e->ch < 0 || e->ch > 7)){
            return 0;
        }
    }
    if(e->freq_valid == 0){
        return 0;
    }
    return 1;
}

void parse_arg(int argc, char **argv, struct env *e)
{
    e->fine_mode = 0; // coarse-mode
    e->adc_mode = 0; // dac-mode
    e->both_mode = 0; // adc- and dac-mode
    e->ch = -1; // default no-settings
    e->freq = 0; // default no-settings
    e->freq_valid = 0; // default no-settings
    e->dac_ch = -1; // default no-settings
    e->adc_ch = -1; // default no-settings
    e->disabled_messages = 0; // enable message
    
    const char * optstring = "h";

    enum {
        HELP = 'h',
        DISABLE_MESSAGES,
        SET_FREQ,
        FINE_MODE,
        ADC_MODE,
        BOTH_MODE,
        SET_CH,
        SET_ADC_CH,
        SET_DAC_CH,
    };
    
    const struct option longopts[] = {
        {"help",            no_argument,       0, HELP},
        {"disable-message", no_argument,       0, DISABLE_MESSAGES},
        {"freq",            required_argument, 0, SET_FREQ},
        {"fine-mode",       no_argument,       0, FINE_MODE},
        {"adc-mode",        no_argument,       0, ADC_MODE},
        {"both-mode",       no_argument,       0, BOTH_MODE},
        {"channel",         required_argument, 0, SET_CH},
        {"adc-channel",     required_argument, 0, SET_ADC_CH},
        {"dac-channel",     required_argument, 0, SET_DAC_CH},
        {0, 0, 0, 0},
    };
    
    int c, longindex;
    while((c = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1){
        switch(c){
        case 'h':
            print_usage();
            exit(0);
            break;
        case DISABLE_MESSAGES:
            e->disabled_messages = 1;
            break;
        case SET_FREQ:
            e->freq = atoll(optarg);
            e->freq_valid = 1; // make e->freq valid
            break;
        case FINE_MODE:
            e->fine_mode = 1;
            break;
        case ADC_MODE:
            e->adc_mode = 1;
            break;
        case BOTH_MODE:
            e->both_mode = 1;
            break;
        case SET_CH:
            e->ch = atoi(optarg);
            break;
        case SET_ADC_CH:
            e->adc_ch = atoi(optarg);
            break;
        case SET_DAC_CH:
            e->dac_ch = atoi(optarg);
            break;
        default:
            print_usage();
            exit(0);
        }
    }

    if(!e->disabled_messages){
        if(e->both_mode){
            printf("set_nco: freq=%ld, %s, %s, adc_ch=%d, dac_ch=%d\n",
                   e->freq,
                   e->fine_mode == 0 ? "COARSE_MODE" : "FINE_MODE",
                   "BOTH_MODE",
                   e->adc_ch,
                   e->dac_ch
            );
        }else{
            printf("set_nco: freq=%ld, ch=%d, %s, %s\n",
                   e->freq,
                   e->ch,
                   e->fine_mode == 0 ? "COARSE_MODE" : "FINE_MODE",
                   e->adc_mode == 1 ? "ADC_MODE" : "DAC_MODE"
            );
        }
    }

    if(validate_env(e) == 0){
        if(!e->disabled_messages){
            print_usage();
        }
        exit(0);
    }
}

int main(int argc, char **argv)
{
    struct env e;
    parse_arg(argc, argv, &e);
    
    adi_ad9081_device_t dev;
    uint64_t dac_clk_hz =     12000000000;
    uint64_t adc_clk_hz =     6000000000;
    uint64_t dev_ref_clk_hz = 12000000000;

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    open_socket(&dev.udp_env_info, target_addr, 16384);

    uint8_t rev_major, rev_minor, rev_rc;
    adi_ad9081_device_api_revision_get(&dev, &rev_major, &rev_minor, &rev_rc);
    SetDevinfo(&dev);
    adi_ad9081_device_init(&dev);
    adi_ad9081_device_clk_config_set(&dev, dac_clk_hz, adc_clk_hz, dev_ref_clk_hz);

    /*-- MAIN FUNCTION ------------------------------------*/
    ad9082_setup(&dev, &e);
    /*---------------------------------------------------- */
    
    if(!e.disabled_messages){
        ad9082_print_info(&dev);
    }

    close_socket(&dev.udp_env_info);
    adi_ad9081_device_hw_close(&dev);

    return 0;
}
