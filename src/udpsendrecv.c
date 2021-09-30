#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "udpsendrecv.h"


#ifdef UDPSENDRECV_MAIN
#define UDPSENDRECV_DEBUG
#endif

static void print_return_packet(unsigned char* buf){
#ifdef UDPSENDRECV_DEBUG
    for(int i = 0; i < 8; i++){
        printf(" %x", buf[i]);
    }
    printf("\n");
#endif
}

int exstickge_ad9082(struct udp_env* env, int cs, int addr, int value, int mode){
    unsigned char mesg[8];
    unsigned char buf[8];
    //printf("AD9082 %d %d %08x %08x\n", mode, cs, addr, value);
    
    mesg[0] = mode == EXSTICKGE_WRITE ? 0x82 : 0x80;
    mesg[1] = EXSTICKGE_AD9082_SPI_CTRL;
    *(int*)(&mesg[2]) = htonl((cs << 15) | (addr & 0x7FFF));
    *(short*)(&mesg[6]) = htons(value);
    
    sendto(env->sock, mesg, 8, 0, (struct sockaddr *)&(env->addr), sizeof(env->addr));
    recv(env->sock, buf, sizeof(buf), 0);
    print_return_packet(buf);
    return (int)ntohs(*(short*)&buf[6]);
}

int exstickge_adrf6780(struct udp_env* env, int cs, int addr, int value, int mode){
    unsigned char mesg[8];
    unsigned char buf[8];
    printf("ADRF6780 %d %d %08x %08x\n", mode, cs, addr, value);
    
    mesg[0] = mode == EXSTICKGE_WRITE ? 0x82 : 0x80;
    mesg[1] = cs > 3 ? EXSTICKGE_ADRF6780_SPI_CTRL_1 : EXSTICKGE_ADRF6780_SPI_CTRL_0;
    *(int*)(&mesg[2]) = htonl(((cs&0x3) << 6) | (addr & 0x3F));
    *(short*)(&mesg[6]) = htons(value);
    
    sendto(env->sock, mesg, 8, 0, (struct sockaddr *)&(env->addr), sizeof(env->addr));
    recv(env->sock, buf, sizeof(buf), 0);
    print_return_packet(buf);
    return (int)ntohs(*(short*)&buf[6]);
}

int exstickge_lmx2594(struct udp_env* env, int cs, int addr, int value, int mode){
    unsigned char mesg[8];
    unsigned char buf[8];
    printf("LMX2594 %d %d %08x %08x\n", mode, cs, addr, value);
    
    mesg[0] = mode == EXSTICKGE_WRITE ? 0x82 : 0x80;
    mesg[1] = cs > 4 ? EXSTICKGE_LMX2594_SPI_CTRL_1 : EXSTICKGE_LMX2594_SPI_CTRL_0;
    *(int*)(&mesg[2]) = htonl(((cs-4) << 7) | (addr & 0x7F));
    *(short*)(&mesg[6]) = htons(value);
    
    sendto(env->sock, mesg, 8, 0, (struct sockaddr *)&(env->addr), sizeof(env->addr));
    recv(env->sock, buf, sizeof(buf), 0);
    print_return_packet(buf);
    return (int)ntohs(*(short*)&buf[6]);
}

int exstickge_ad5328(struct udp_env* env, int addr, int value, int mode){
    unsigned char mesg[8];
    unsigned char buf[8];
    printf("AD5328 %d %08x %08x\n", mode, addr, value);
    
    mesg[0] = mode == EXSTICKGE_WRITE ? 0x82 : 0x80;
    mesg[1] = EXSTICKGE_AD5328_SPI_CTRL;
    *(int*)(&mesg[2]) = htonl(addr);
    *(short*)(&mesg[6]) = htons(value);
    
    sendto(env->sock, mesg, 8, 0, (struct sockaddr *)&(env->addr), sizeof(env->addr));
    recv(env->sock, buf, sizeof(buf), 0);
    print_return_packet(buf);
    return (int)ntohs(*(short*)&buf[6]);
}

int exstickge_gpio(struct udp_env* env, int value, int mode){
    unsigned char mesg[8];
    unsigned char buf[8];
    printf("GPIO %d %08x\n", mode, value);
    
    mesg[0] = mode == EXSTICKGE_WRITE ? 0x82 : 0x80;
    mesg[1] = EXSTICKGE_GPIO;
    *(int*)(&mesg[2]) = htonl(0x00000000);
    *(short*)(&mesg[6]) = htons(value);
    
    sendto(env->sock, mesg, 8, 0, (struct sockaddr *)&(env->addr), sizeof(env->addr));
    recv(env->sock, buf, sizeof(buf), 0);
    print_return_packet(buf);
    return (int)ntohs(*(short*)&buf[6]);
}

void open_socket(struct udp_env* env, char *addr, int port){
    env->sock = socket(AF_INET, SOCK_DGRAM, 0);
    env->addr.sin_family = AF_INET;
    env->addr.sin_port = htons(port);
    env->addr.sin_addr.s_addr = inet_addr(addr);
    return;
}

void close_socket(struct udp_env* env){
    close(env->sock);
    return;
}


#ifdef UDPSENDRECV_MAIN
#ifndef TARGET_ADDRESS
#define TARGET_ADDRESS "10.0.0.3"
#endif
#ifndef TARGET_PORT
#define TARGET_PORT 16384
#endif
void print_usage(char *cmd){
    printf("usage: %s command args...\n", cmd);
    printf(" ad9082 w/r chip addr value\n");
    printf(" adrf6780 w/r chip addr value\n");
    printf(" lmx2594 w/r chip addr value\n");
    printf(" ad5328 w/r addr value\n");
    printf(" gpio w/r value\n");
    printf("chip should be decimal, addr and value should be hex\n");
}

int main(int argc, char **argv)
{
    char *target_addr = TARGET_ADDRESS;
    char *val = getenv("TARGET_ADDR");
    if(val != NULL){
        target_addr = val;
    }

    struct udp_env env;
    open_socket(&env, target_addr, TARGET_PORT);

    if(argc < 3){
        print_usage(argv[0]);
        close_socket(&env);
        return 0;
    }

    int mode = EXSTICKGE_WRITE;
    if(strcmp(argv[2], "r") == 0){
        mode = EXSTICKGE_READ;
    }else if(strcmp(argv[2], "w") == 0){
        mode = EXSTICKGE_WRITE;
    }else{
        printf("unknown r/w mode %s\n", argv[2]);
        print_usage(argv[0]);
        close_socket(&env);
        return 0;
    }

    char *endptr;
    int retval = 0;

    if(strcmp(argv[1], "ad9082") == 0 && argc == 6){
        int chip = atoi(argv[3]);
        int addr = strtol(argv[4], &endptr, 16);
        int value = strtol(argv[5], &endptr, 16);
        retval = exstickge_ad9082(&env, chip, addr, value, mode);
    }else if(strcmp(argv[1], "adrf6780") == 0 && argc == 6){
        int chip = atoi(argv[3]);
        int addr = strtol(argv[4], &endptr, 16);
        int value = strtol(argv[5], &endptr, 16);
        retval = exstickge_adrf6780(&env, chip, addr, value, mode);
    }else if(strcmp(argv[1], "lmx2594") == 0 && argc == 6){
        int chip = atoi(argv[3]);
        int addr = strtol(argv[4], &endptr, 16);
        int value = strtol(argv[5], &endptr, 16);
        retval = exstickge_lmx2594(&env, chip, addr, value, mode);
    }else if(strcmp(argv[1], "ad5328") == 0 && argc == 5){
        int addr = strtol(argv[3], &endptr, 16);
        int value = strtol(argv[4], &endptr, 16);
        retval = exstickge_ad5328(&env, addr, value, mode);
    }else if(strcmp(argv[1], "gpio") == 0 && argc == 4){
        int value = strtol(argv[3], &endptr, 16);
        retval = exstickge_gpio(&env, value, mode);
    }else{
        printf("unknown command %s\n", argv[1]);
        print_usage(argv[0]);
    }
    printf("return value: %08x\n", retval);
    
    close_socket(&env);
    return 0;
}
#endif
