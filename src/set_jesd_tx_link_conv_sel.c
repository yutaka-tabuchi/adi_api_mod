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
    uint8_t conv_sel[16];
};

void ad9082_setup(adi_ad9081_device_t *ad9081_dev, struct env *e)
{
    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2]=
        {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
    int i;
    uint8_t *ptr = &jesd_conv_sel[0].virtual_converter0_index;
    for(i = 0; i < 16; i++){
        ptr[i] = e->conv_sel[i];
    }

    // same setting with helloworld.c
    adi_cms_jesd_param_t jesd_param[2] =
    //  L  F   M  S HD   K   N  NP CF CS DID BID LID SCL SCR DUAL  B/C  ID  C2R   S
    { { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0,  0,  0,  0,  0,  1,   0,   2, 17,   0,  0 },
      { 8, 4, 16, 1, 0, 64, 16, 16, 0, 0,  0,  0,  0,  0,  1,   0,   2, 17,   0,  0 } };

    uint8_t links = jesd_param[0].jesd_duallink > 0 ? AD9081_LINK_ALL : AD9081_LINK_0;
    uint8_t jesd_m[2] = { jesd_param[0].jesd_m, jesd_param[1].jesd_m };
    int32_t err = adi_ad9081_jesd_tx_link_conv_sel_set(ad9081_dev, links, jesd_conv_sel, jesd_m);
}

void print_usage()
{
    printf("usage: set_jesd_tx_link_conv_sel\n");
    printf("\t--help           \tprint this message\n");
    printf("\t--args           \tjsed_conv_sel pattern(ex. 2,3,0,1,6,7,4,5,10,11,8,9,14,15,12,13 )\n");
    printf("\t--disable-message\tsuppress human readable message\n");
}

int validate_env(struct env *e)
{
    return 1;
}

void parse_arg(int argc, char **argv, struct env *e)
{
    e->disabled_messages = 0; // enable message
    int i;
    for(i = 0; i < 16; i++){
        e->conv_sel[i] = i;
    }
    
    const char * optstring = "h";

    enum {
        HELP = 'h',
        DISABLE_MESSAGES,
        SET_ARGS,
    };
    
    const struct option longopts[] = {
        {"help",            no_argument,       0, HELP},
        {"disable-message", no_argument,       0, DISABLE_MESSAGES},
        {"args",           required_argument,  0, SET_ARGS},
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
        case SET_ARGS: {
            int i = 0;
            char* p = strtok(optarg, ",");
            while (p != NULL) {
                if(i < 16){
                    e->conv_sel[i] = atoi(p); i++;
                }
                p = strtok(NULL, ",");
            }
        }
            break;
        default:
            print_usage();
            exit(0);
        }
    }

    if(!e->disabled_messages){
        printf("set_jesd_tx_link_conv_sel: args=");
        int i;
        char *sep = "{";
        for(i = 0; i < 16; i++){
            printf("%s%d", sep, e->conv_sel[i]);
            sep = ", ";
        }
        printf("}\n");
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
