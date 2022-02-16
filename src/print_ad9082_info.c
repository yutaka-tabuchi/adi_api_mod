#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "util.h"
#include "udpsendrecv.h"

void ad9082_setup(adi_ad9081_device_t *dev)
{
    ad9082_print_info(dev);
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
