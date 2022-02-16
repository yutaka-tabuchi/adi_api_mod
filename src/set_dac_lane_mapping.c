#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "util.h"
#include "udpsendrecv.h"

int main(int argc, char **argv)
{
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

    int status;
    int value = atoi(argv[1]);
    for(int i = 0; i < 8; i++){
       status = adi_ad9081_jesd_rx_lane_xbar_set(&ad9081_dev, AD9081_LINK_0, value, i);
       printf("status=%d\n", status);
    }

    ad9082_print_info(&ad9081_dev);

    close_socket(&ad9081_dev.udp_env_info);
    adi_ad9081_device_hw_close(&ad9081_dev);

    return 0;
}
