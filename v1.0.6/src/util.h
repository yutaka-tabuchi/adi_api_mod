#ifndef _UTIL_H_
#define _UTIL_H_

void SetDevinfo(adi_ad9081_device_t *ad9081_dev);
void ad9082_print_info(adi_ad9081_device_t *ad9081_dev);
int ad9082_chip();
int ad9082_adc_scramble();
void print_reg(adi_ad9081_device_t *ad9081_dev, int addr);

#endif /* _UTIL_H_ */

