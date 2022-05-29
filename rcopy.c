// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "networks.h"
#include "cpe464.h"
#include "srej.h"

typedef enum State STATE;

enum State{
    DONE, FILENAME, RECV_DATA, FILEOK, START_STATE
};

void processFile(char* argv[]);
STATE start_state(char **argv, Connection * server, uint32_t * clientSeqNum);
STATE filename(char * fname, int32_t buf_size, Connection * server);
STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum);
STATE file_ok(int * outputFileFd, char* outputFileName);
void check_args(int arc, char * argv[]);

int main(int argc, char * argv[]){
    check_args(argc, argv);
    sendtoErr_init(atof(argv[4]), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_ON);
    processFile(argv);
    return 0;
}

void processFile(char* argv[]){
    Connection * server = (Connection *) calloc(1, sizeof(Connection));
    uint32_t clientSeqNum = 0;
    int32_t output_file_fd = 0;
    STATE state = START_STATE;

    while(state != DONE){
        switch (state) {
            case START_STATE:
                printf("in start\n");
                state = start_state(argv, server, &clientSeqNum);
                break;

            case FILENAME:
                printf("in filename\n");
                state = filename(argv[1], atoi(argv[3]), server);
                break;

            case FILEOK:
                state = file_ok(&output_file_fd, argv[2]);
                break;

            case RECV_DATA:
                state = recv_data(output_file_fd, server, &clientSeqNum);
                break;

            case DONE:
                printf("Process Completed\n");
                break;

            default:
                printf("ERROR - default state\n");
                state = DONE;
                break;
        }
    }
}

STATE start_state(char **argv, Connection * server, uint32_t * clientSeqNum){
    uint8_t packet[MAX_LEN];
    uint8_t buf[MAX_LEN];
    int fileNameLen = strlen(argv[1]);
    STATE returnValue = FILENAME;
    uint32_t bufferSize = 0;

    if(server->sk_num > 0){
        close(server->sk_num);
    }

    if(udpClientSetup(argv[5], atoi(argv[6]), server) < 0){
        returnValue = DONE;//UDP client unsuccessful
    }
    else{
        bufferSize = htonl(atoi(argv[3]));
        memcpy(buf, &bufferSize, SIZE_OF_BUF);
        memcpy(&buf[SIZE_OF_BUF], argv[1], fileNameLen);
        printIPv6Info(&server->remote);
        send_buf(buf, fileNameLen + SIZE_OF_BUF, server, FNAME, *clientSeqNum, packet);
        (*clientSeqNum)++;

        returnValue = FILENAME;
    }

    return returnValue;
}

STATE filename(char* fname, int32_t buf_size, Connection * server){
    int returnValue = START_STATE;
    uint8_t packet[MAX_LEN];
    uint8_t flag = 0;
    uint32_t seq_num = 0;
    int32_t recv_check = 0;
    static int retryCount = 0;

    if((returnValue = processSelect(server, &retryCount, START_STATE, FILEOK, DONE)) == FILEOK){
        recv_check = recv_buf(packet, MAX_LEN, server->sk_num, server, &flag, &seq_num);
        if(recv_check == CRC_ERROR){
            returnValue = START_STATE;
        }
        else if(flag == FNAME_BAD){
            printf("File %s not found\n", fname);
            returnValue = DONE;
        }
        else if(flag == DATA){
            returnValue = FILEOK;
        }
    }

    return returnValue;
}

STATE file_ok(int *outputFileFd, char* outputFileName){
    STATE returnValue = DONE;
    if((*outputFileFd = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0){
        perror("file open error: ");
        returnValue = DONE;
    }
    else{
        printf("file created successfully: %s\n", outputFileName);
        returnValue = RECV_DATA;
    }

    return returnValue;
}

STATE recv_data(int32_t output_file, Connection * server, uint32_t * clientSeqNum){
    uint32_t    seq_num = 0;
    uint32_t    ackSeqNum = 0;
    uint8_t     flag = 0;
    int32_t     data_len = 0;
    uint8_t     data_buf[MAX_LEN];
    uint8_t     packet[MAX_LEN];
    static      int32_t expected_seq_num = START_SEQ_NUM;

    if(select_call(server->sk_num, LONG_TIME, 0) == 0){
        printf("Timeout after 10 seconds, server must be gone.\n");
        return DONE;
    }

    data_len = recv_buf(data_buf, MAX_LEN, server->sk_num, server, &flag, &seq_num);

    if(data_len == CRC_ERROR){
        return RECV_DATA;
    }
    if(flag == END_OF_FILE){
        send_buf(packet, 1, server, EOF_ACK, *clientSeqNum, packet);
        (*clientSeqNum)++;
        printf("File done\n");
        return DONE;
    }
    else{
        ackSeqNum = htonl(seq_num);
        send_buf((uint8_t *)&ackSeqNum, sizeof(ackSeqNum), server, ACK, *clientSeqNum, packet);
        (*clientSeqNum)++;
    }

    if(seq_num == expected_seq_num){
        expected_seq_num++;
        write(output_file, &data_buf, data_len);
    }
    return RECV_DATA;
}

void check_args(int argc, char ** argv){
    if(argc != 7){
        printf("Usage %s fromFile toFile buffer_size error_rate hostname port\n", argv[0]);
        exit(-1);
    }

    if(strlen(argv[1]) > 1000){
        printf("FROM filename too long. Needs to be less than 1000 and is: %ld\n", strlen(argv[1]));
        exit(-1);
    }

    if(strlen(argv[2]) > 1000){
        printf("FROM filename too long. Needs to be less than 1000 and is: %ld\n", strlen(argv[2]));
        exit(-1);
    }

    if(atoi(argv[3]) < 400 || atoi(argv[3]) > 1400){
        printf("Buffer size needs to be between 400 and 1400 and is: %d\n", atoi(argv[3]));
        exit(-1);
    }

    if(atoi(argv[4]) < 0 || atoi(argv[4]) >= 1){
        printf("Error rate needs to be between 0 and less than 1 and is: %d\n", atoi(argv[4]));
        exit(-1);
    }
}




