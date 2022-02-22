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
    int ch;
    int uA;
    int rerun_cal;
};

void ad9082_setup(adi_ad9081_device_t *dev, struct env *e)
{
    int32_t err;
    err = adi_ad9081_dac_fsc_set(dev, AD9081_DAC_0 << e->ch, e->uA, e->rerun_cal);
}

void print_usage()
{
    printf("usage: set_xbar\n");
    printf("\t--help           \tprint this message\n");
    printf("\t--channel        \tset target channel (between 0 and 3)\n");
    printf("\t--value          \tassign digital channel mask(hex string, between 00 to FF)\n");
    printf("\t--disable-message\tsuppress human readable message\n");
}

int validate_env(struct env *e)
{
    if(e->ch < 0 || e->ch > 3){
        return 0;
    }
    if(e->uA < 7000 || e->uA > 40000){
        return 0;
    }
    if(e->rerun_cal < 0 || e->rerun_cal > 1){
        return 0;
    }
    return 1;
}

void parse_arg(int argc, char **argv, struct env *e)
{
    e->disabled_messages = 0; // enable message
    e->ch = -1; // default no-settings
    e->uA = -1; // default no-settings
    e->rerun_cal = 0;
    
    const char * optstring = "h";

    enum {
        HELP = 'h',
        DISABLE_MESSAGES,
        SET_CH,
        SET_VALUE,
        RERUN_CAL,
    };
    
    const struct option longopts[] = {
        {"help",            no_argument,       0, HELP},
        {"disable-message", no_argument,       0, DISABLE_MESSAGES},
        {"channel",         required_argument, 0, SET_CH},
        {"value",           required_argument, 0, SET_VALUE},
        {"rerun-cal",       no_argument,       0, RERUN_CAL},
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
        case SET_CH:
            e->ch = atoi(optarg);
            break;
        case SET_VALUE:
            e->uA = atoi(optarg);
            break;
        case RERUN_CAL:
            e->rerun_cal = 1;
            break;
        default:
            print_usage();
            exit(0);
        }
    }

    if(!e->disabled_messages){
        printf("set_fcs: ch=%d, uA=%02x, rerun_cal=%d\n", e->ch, e->uA, e->rerun_cal);
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
    uint64_t adc_clk_hz =     3000000000;
    uint64_t dev_ref_clk_hz = 12000000000;

    char *target_addr = "10.0.0.3";
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }
    //printf("target addr:%s\n", target_addr);
    open_socket(&ad9081_dev.udp_env_info, target_addr, 16384);

    uint8_t rev_major, rev_minor, rev_rc;
    adi_ad9081_device_api_revision_get(&ad9081_dev, &rev_major, &rev_minor, &rev_rc);
    SetDevinfo(&ad9081_dev);
    adi_ad9081_device_init(&ad9081_dev);
    adi_ad9081_device_clk_config_set(&ad9081_dev, dac_clk_hz, adc_clk_hz, dev_ref_clk_hz);

    /*-- MAIN FUNCTION ------------------------------------*/
    ad9082_setup(&ad9081_dev, &e);
    /*---------------------------------------------------- */

    if(!e.disabled_messages){
        ad9082_print_info(&ad9081_dev);
    }

    close_socket(&ad9081_dev.udp_env_info);
    adi_ad9081_device_hw_close(&ad9081_dev);

    return 0;
}
