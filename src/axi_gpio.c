#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "axi_gpio.h"

#define PAGESIZE (4096)
#define PAGEMASK (0xFFFFF000)

int axi_gpio_open(struct axi_gpio_env_t* env, unsigned int addr)
{
    char *dev = "/dev/mem";
    
    env->fd = open(dev, O_RDWR);
    if(env->fd < 0){
        fprintf(stderr,"cannot open %s\n", dev);
        return -1;
    }
    
    env->base = addr & PAGEMASK;
    env->len = PAGESIZE;
#ifdef AXI_GPIO_MAIN
    printf("begin = %08x, len = %08x\n", env->base, env->len);
#endif
    
    env->mm = mmap(0, env->len, PROT_WRITE, MAP_SHARED, env->fd, env->base);
    if(env->mm == MAP_FAILED){
        fprintf(stderr,"cannot mmap\n");
        return -1;
    }
    
    return 0;
}

void axi_gpio_close(struct axi_gpio_env_t* env)
{
    munmap(env->mm, env->len);
    close(env->fd);
}

void axi_gpio_write(struct axi_gpio_env_t* env, unsigned int offset, int value)
{
    offset = (offset+4) <= PAGESIZE ? offset : PAGESIZE-4;
#ifdef AXI_GPIO_MAIN
    printf("byte offset = %08x\n", offset);
#endif
    int *ptr = (int*)(env->mm + offset);
    *ptr = value;
}

int axi_gpio_read(struct axi_gpio_env_t* env, unsigned int offset)
{
    offset = (offset+4) <= PAGESIZE ? offset : PAGESIZE-4;
#ifdef AXI_GPIO_MAIN
    printf("byte offset = %08x\n", offset);
#endif
    int *ptr = (int*)(env->mm + offset);
    return *ptr;
}

int axi_gpio_write_once(unsigned int addr, int value)
{
    struct axi_gpio_env_t env;
    int ret = axi_gpio_open(&env, addr);
    if(ret < 0){
        return ret;
    }
    unsigned int offset = addr - env.base;
    axi_gpio_write(&env, offset, value);
    axi_gpio_close(&env);
    return 0;
}

int axi_gpio_read_once(unsigned int addr, int *value)
{
    struct axi_gpio_env_t env;
    int ret = axi_gpio_open(&env, addr);
    if(ret < 0){
        return ret;
    }
    unsigned int offset = addr - env.base;
    *value = axi_gpio_read(&env, offset);
    axi_gpio_close(&env);
    return 0;
}

#ifdef AXI_GPIO_MAIN
int main(int argc, char** argv)
{
    struct axi_gpio_env_t env;
    if(argc < 3)
    {
        printf("usage: %s write|read addr [data]\n", argv[0]);
        return 1;
    }
    int addr = strtoul(argv[2], NULL, 16);
    int data = 0;
    if(strcmp(argv[1], "write") == 0){
        if(argc < 4){
            printf("write command requires data\n");
            return 1;
        }
        data = strtoul(argv[3], NULL, 16);
    }

    if(strcmp(argv[1], "write") == 0){
        axi_gpio_write_once(addr, data);
        printf("gpio write: [%08x] <= [%08x]\n", addr, data);
    }else{
        int v;
        axi_gpio_read_once(addr, &v);
        printf("gpio read: [%08x] => [%08x]\n", addr, v);
    }
    return 0;
}
#endif
