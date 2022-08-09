#ifndef __UDPSENDRECV__
#define __UDPSENDRECV__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define EXSTICKGE_AD9082_SPI_CTRL     (1)
#define EXSTICKGE_ADRF6780_SPI_CTRL_0 (2)
#define EXSTICKGE_ADRF6780_SPI_CTRL_1 (3)
#define EXSTICKGE_LMX2594_SPI_CTRL_0  (4)
#define EXSTICKGE_LMX2594_SPI_CTRL_1  (5)
#define EXSTICKGE_AD5328_SPI_CTRL     (6)
#define EXSTICKGE_GPIO                (7)
#define EXSTICKGE_AD7490_SPI_CTRL     (8)
#define EXSTICKGE_USER_CTRL           (9)

#define EXSTICKGE_WRITE (0)
#define EXSTICKGE_READ (1)

struct udp_env{
    int sock;
    struct sockaddr_in addr;
};

int exstickge_ad9082(struct udp_env* env, int cs, int addr, int value, int mode);
int exstickge_adrf6780(struct udp_env* env, int cs, int addr, int value, int mode);
int exstickge_lmx2594(struct udp_env* env, int cs, int addr, int value, int mode);
int exstickge_ad5328(struct udp_env* env, int addr, int value, int mode);
int exstickge_ad5328_quel2(struct udp_env* env, int cs, int addr, int value, int mode);
int exstickge_gpio(struct udp_env* env, int value, int mode);
int exstickge_ad7490(struct udp_env* env, int cs, int addr, int value, int mode);
int exstickge_userctrl(struct udp_env* env, int value, int mode);
void open_socket(struct udp_env* env, char *addr, int port);
void close_socket(struct udp_env* env);

#endif /* __UDPSENDRECV__ */

