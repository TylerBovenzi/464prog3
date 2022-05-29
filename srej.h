#ifndef SREJ_H
#define SREJ_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "window.h"
#include "networks.h"

#define MAX_LEN 1500
#define SIZE_OF_BUF 4
#define START_SEQ_NUM 1
#define MAX_TRIES 10
#define LONG_TIME 10
#define SHORT_TIME 1

typedef struct header Header;

struct header{
    uint32_t seq_num;
    uint16_t checksum;
    uint8_t flag;
};

enum FLAG{
    FNAME = 7, DATA = 3, FNAME_OK = 8, FNAME_BAD = 9, ACK = 5, END_OF_FILE = 10, EOF_ACK = 11, CRC_ERROR = -1
};

int32_t send_buf(uint8_t* buf, uint32_t len, Connection * connection, uint8_t flag, uint32_t seq_num, uint8_t *packet);
int createHeader(uint32_t len, uint8_t flag, uint32_t seq_num, uint8_t* packet);
int32_t recv_buf(uint8_t* buf, int32_t len, int32_t recv_sk_num, Connection* connection, uint8_t *flag, uint32_t * seq_num);
int retrieveHeader(uint8_t* data_buf, int recv_len, uint8_t * flag, uint32_t * seq_num);
int processSelect(Connection * client, int * retry_count, int selectTimeoutState, int dataReadyState, int doneState);
int32_t store_buf(struct window * win,uint8_t* buf, uint32_t len, uint8_t flag, uint32_t seq_num);
#endif