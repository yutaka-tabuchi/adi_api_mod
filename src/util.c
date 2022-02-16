#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "adi_ad9081_config.h"
#include "adi_ad9081_hal.h"
#define MHZ 1000000

#include "udpsendrecv.h"

#include "util.h"

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
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].post_emp_setting=AD9081_SER_POST_EMP_6DB;
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].pre_emp_setting=AD9081_SER_PRE_EMP_6DB;
        ad9081_dev->serdes_info.ser_settings.lane_settings[i].swing_setting=AD9081_SER_SWING_1000;
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

void print_reg(adi_ad9081_device_t *ad9081_dev, int addr)
{
    uint8_t reg_data;
    adi_ad9081_hal_reg_get(ad9081_dev, addr, &reg_data);
    printf("0x%04X = 0x%02X\n", addr, reg_data);
}
