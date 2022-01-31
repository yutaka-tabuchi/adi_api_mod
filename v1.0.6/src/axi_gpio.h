#ifndef __AXI_GPIO_H__
#define __AXI_GPIO_H__

struct axi_gpio_env_t{
    int fd;
    char *mm;
    unsigned int base;
    int len;
};

int axi_gpio_open(struct axi_gpio_env_t* env, unsigned int addr);
void axi_gpio_close(struct axi_gpio_env_t* env);
void axi_gpio_write(struct axi_gpio_env_t* env, unsigned int addr, int value);
int axi_gpio_read(struct axi_gpio_env_t* env, unsigned int addr);
int axi_gpio_write_once(unsigned int addr, int value);
int axi_gpio_read_once(unsigned int addr, int *value);

#endif /* __AXI_GPIO_H__ */
