#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#define PAGESIZE (4096)
#define PAGEMASK (0xFFFFF000)

int axi_gpio_write(int addr, int value)
{
    char *dev = "/dev/mem";
    
    char *mm;
    int fd;

    int beg = addr & PAGEMASK;
    int len = 4096;

    fd = open(dev, O_RDWR);
    if(fd < 0){
        fprintf(stderr,"cannot open %s\n", dev);
        return -1;
    }

    printf("begin = %08x, len = %08x\n", beg, len);
    mm = mmap(0, len, PROT_WRITE, MAP_SHARED, fd, beg);
    if(mm == MAP_FAILED){
        fprintf(stderr,"cannot mmap\n");
        return 1;
    }
    
    int *ptr = (int*)(mm + (addr - beg));
    *ptr = value;
    
    munmap(mm, len);
    close(fd);
    return 0;
}

int axi_gpio_read(int addr, int* value)
{
    char *dev = "/dev/mem";
    
    char *mm;
    int fd;

    unsigned int beg = addr & PAGEMASK;
    int len = 4096;

    fd = open(dev, O_RDWR);
    if(fd < 0){
        fprintf(stderr,"cannot open %s\n", dev);
        return -1;
    }
    
    printf("begin = %08x, len = %08x\n", beg, len);
    mm = mmap(0, len, PROT_READ, MAP_SHARED, fd, beg);
    if(mm == MAP_FAILED){
        fprintf(stderr,"cannot mmap\n");
        return 1;
    }
    
    int *ptr = (int*)(mm + (addr - beg));
    *value = *ptr;
    
    munmap(mm, len);
    close(fd);
    return 0;
}

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        return 1;
    }
    int addr = strtoul(argv[1], NULL, 16);
    
    for(int i = 0; i < 16; i++){
        int v;
        axi_gpio_read(addr+i*4, &v);
        printf("%08x\n", v);
    }
    
    return 0;
}

