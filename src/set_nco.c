#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "udpsendrecv.h"
#include "util.h"

void ad9082_setup(adi_ad9081_device_t *ad9081_dev, int fine, int ch, int64_t freq)
{
    int32_t err;
    if(fine == 0){
        err = adi_ad9081_dac_duc_nco_set(ad9081_dev, AD9081_DAC_0 << ch, AD9081_DAC_CH_NONE, freq);
    }else{
        err = adi_ad9081_dac_duc_nco_set(ad9081_dev, AD9081_DAC_NONE, AD9081_DAC_CH_0 << ch, freq);
    }
}

void print_usage()
{
    printf("usage: set_nco\n");
    printf("\t--help          \tprint this message\n");
    printf("\t--freq          \tset frequency(> 0)\n");
    printf("\t--channel       \tset target channel (between 0 and 7 for fine NCO, between 0 and 3 for corase NCO)\n");
    printf("\t--fine-mode     \tto set fine NCO(default; coarse NCO)\n");
    printf("\t--disable-message\tsuppress human readable message\n");
}

struct env {
    int fine_mode;
    int ch;
    int64_t freq;
    int disabled_messages;
};

int validate_env(struct env *e)
{
    if(e->fine_mode == 0 && (e->ch < 0 || e->ch > 3)){
        return 0;
    }
    if(e->fine_mode == 1 && (e->ch < 0 || e->ch > 7)){
        return 0;
    }
    if(e->freq < 0){
        return 0;
    }
    return 1;
}

void parse_arg(int argc, char **argv, struct env *e)
{
    e->fine_mode = 0; // coarse-mode
    e->ch = -1; // default no-settings
    e->freq = -1; // default no-settings
    e->disabled_messages = 0; // enable message
    
    const char * optstring = "h";

    enum {
        HELP = 'h',
        SET_FREQ,
        FINE_MODE,
        SET_CH,
        DISABLE_MESSAGES,
    };
    
    const struct option longopts[] = {
        {"help",            no_argument,       0, HELP},
        {"freq",            required_argument, 0, SET_FREQ},
        {"fine-mode",       no_argument,       0, FINE_MODE},
        {"channel",         required_argument, 0, SET_CH},
        {"disable-message", no_argument,       0, DISABLE_MESSAGES},
        {0, 0, 0, 0},
    };
    
    int c, longindex;
    while((c = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1){
        switch(c){
        case 'h':
            print_usage();
            exit(0);
            break;
        case SET_FREQ:
            e->freq = atoll(optarg);
            break;
        case FINE_MODE:
            e->fine_mode = 1;
            break;
        case SET_CH:
            e->ch = atoi(optarg);
            break;
        case DISABLE_MESSAGES:
            e->disabled_messages = 1;
            break;
        default:
            print_usage();
            exit(0);
        }
    }

    if(!e->disabled_messages){
        printf("set_nco: freq=%ld, ch=%d, %s\n",
               e->freq,
               e->ch,
               e->fine_mode == 0 ? "COARSE_MODE" : "FINE_MODE");
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
    
    adi_ad9081_device_t ad9081_dev;
    uint64_t dac_clk_hz =     12000000000;
    uint64_t adc_clk_hz =     6000000000;
    uint64_t dev_ref_clk_hz = 12000000000;

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);

    uint8_t rev_major, rev_minor, rev_rc;
    adi_ad9081_device_api_revision_get(&ad9081_dev, &rev_major, &rev_minor, &rev_rc);
    SetDevinfo(&ad9081_dev);
    adi_ad9081_device_init(&ad9081_dev);
    adi_ad9081_device_clk_config_set(&ad9081_dev, dac_clk_hz, adc_clk_hz, dev_ref_clk_hz);

    /*-- MAIN FUNCTION ------------------------------------*/
    
    ad9082_setup(&ad9081_dev, e.fine_mode, e.ch, e.freq);
    
    /*---------------------------------------------------- */
    
    if(!e.disabled_messages){
        ad9082_print_info(&ad9081_dev);
    }

    close_socket(&ad9081_dev.udp_env_info);
    adi_ad9081_device_hw_close(&ad9081_dev);

    return 0;
}
