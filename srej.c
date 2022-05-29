#include "srej.h"
#include "networks.h"
#include "checksum.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "window.h"
#include "networks.h"
#include "srej.h"
#include "checksum.h"

int32_t send_buf(uint8_t* buf, uint32_t len, Connection * connection, uint8_t flag, uint32_t seq_num, uint8_t *packet){
    int32_t sentLen = 0;
    int32_t sendingLen = 0;

    if(len > 0){
        memcpy(&packet[sizeof(Header)], buf, len);
    }

    sendingLen = createHeader(len, flag, seq_num, packet);

    sentLen = safeSendto(packet, sendingLen, connection);

    return sentLen;
}

int32_t store_buf(struct window * win,uint8_t* buf, uint32_t len, uint8_t flag, uint32_t seq_num){
    int32_t sentLen = 0;
    int32_t sendingLen = 0;
    uint8_t packet[MAX_LEN];
    if(len > 0){
        memcpy(&packet[sizeof(Header)], buf, len);
    }

    sendingLen = createHeader(len, flag, seq_num, packet);

    enqueue(win, seq_num, len, packet);

    return sentLen;
}

int createHeader(uint32_t len, uint8_t flag, uint32_t seq_num, uint8_t* packet){
    Header * aHeader = (Header *) packet;
    uint16_t checksum = 0;
    seq_num = ntohl(seq_num);
    memcpy(&(aHeader->seq_num), &seq_num, sizeof(seq_num));

    aHeader->flag = flag;

    memset(&(aHeader->checksum), 0, sizeof(checksum));
    checksum = in_cksum((unsigned short *) packet, len + sizeof(Header));
    memcpy(&(aHeader->checksum), &checksum, sizeof(checksum));

    return len + sizeof(Header);
}

int32_t recv_buf(uint8_t* buf, int32_t len, int32_t recv_sk_num, Connection* connection, uint8_t *flag, uint32_t *seq_num){
    uint8_t data_buf[MAX_LEN];
    int32_t recv_len = 0;
    int32_t dataLen = 0;

    recv_len = safeRecvfrom(recv_sk_num, data_buf, len, connection);

    dataLen = retrieveHeader(data_buf, recv_len, flag, seq_num);

    if(dataLen > 0)
        memcpy(buf, &data_buf[sizeof(Header)], dataLen);

    return dataLen;
}

int retrieveHeader(uint8_t* data_buf, int recv_len, uint8_t * flag, uint32_t * seq_num){
    Header * aHeader = (Header *) data_buf;
    int returnValue = 0;

    if(in_cksum((unsigned short *) data_buf, recv_len) != 0){
        returnValue = CRC_ERROR;
    }
    else{
        *flag = aHeader->flag;
        memcpy(seq_num, &(aHeader->seq_num), sizeof(aHeader->seq_num));
        *seq_num = ntohl(*seq_num);

        returnValue = recv_len - sizeof(Header);
    }

    return returnValue;
}

int processSelect(Connection * client, int * retry_count, int selectTimeoutState, int dataReadyState, int doneState){
    int returnValue = dataReadyState;

    (*retry_count)++;
    if(*retry_count > MAX_TRIES){
        printf("No response for other side for %d seconds, terminating connection\n", MAX_TRIES);
        returnValue = doneState;
    }
    else{
        if(select_call(client->sk_num, SHORT_TIME, 0) == 1){
            *retry_count = 0;
            returnValue = dataReadyState;
        }
        else{
            returnValue = selectTimeoutState;
        }
    }

    return returnValue;
}
