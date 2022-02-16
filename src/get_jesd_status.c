#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "util.h"
#include "udpsendrecv.h"

void ad9082_setup(adi_ad9081_device_t *ad9081_dev)
{
    int i;
    uint8_t reg_data;

    for(i=0;i<4;i++){
    	adi_ad9081_hal_reg_get(ad9081_dev,0x55E,&reg_data);
    	printf("0x55E=%X\n",reg_data);
        adi_ad9081_hal_reg_get(ad9081_dev, 0x5BB, &reg_data);
        printf("0x5BB=%X\n", reg_data);
        adi_ad9081_hal_reg_get(ad9081_dev, 0x62E, &reg_data);
        printf("0x62E=%X\n", reg_data);
    }

    uint8_t pll_status;
    adi_ad9081_jesd_pll_lock_status_get(ad9081_dev, &pll_status);
    printf("pll_status=%02x\n", pll_status);

    //adi_ad9081_device_direct_loopback_set(ad9081_dev, 1, 0xFF);
    //adi_ad9081_jesd_loopback_mode_set(ad9081_dev, 1);
    adi_ad9081_hal_reg_get(ad9081_dev, 0x728, &reg_data);
    printf("0x728=%X\n", reg_data);
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
