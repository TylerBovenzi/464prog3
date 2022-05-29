#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "cpe464.h"
#include "networks.h"

int safeGetUDPSocket(){
    int socketNumber = 0;
    if((socketNumber = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
        perror("safeGetUdpSocket(), socket() call: ");
        exit(-1);
    }

    return socketNumber;
}

int udpServerSetup(int portNumber){
    struct sockaddr_in6 server;
    int socketNumber = 0;
    int serverAddrLen = 0;

    socketNumber = safeGetUDPSocket();

    server.sin6_family = AF_INET6;
    server.sin6_addr = in6addr_any;
    server.sin6_port = htons(portNumber);

    if(bind(socketNumber, (struct sockaddr *) &server, sizeof(server)) < 0){
        //printf("bind sucks!\n");
        perror("bind() call error");
        exit(-1);
    }

    //printf("");

    serverAddrLen = sizeof(server);
    getsockname(socketNumber, (struct sockaddr *) &server, (socklen_t *)&serverAddrLen);
    printf("Server using port #: %d\n", ntohs(server.sin6_port));

    return socketNumber;
}

int udpClientSetup(char * hostname, int portNumber, Connection * connection){
    memset(&connection->remote, 0, sizeof(struct sockaddr_in6));
    connection->sk_num = 0;
    connection->len = sizeof(struct sockaddr_in6);
    connection->remote.sin6_family = AF_INET6;
    connection->remote.sin6_port = htons(portNumber);

    connection->sk_num = safeGetUDPSocket();

    if(gethostbyname6(hostname, &connection->remote) == NULL){
        printf("Host not found: %s\n", hostname);
        return -1;
    }

    printf("Server Info - ");
    printIPv6Info(&connection->remote);

    return 0;
}

int select_call(int32_t socket_num, int32_t seconds, int32_t microseconds){
    fd_set fdvar;
    struct timeval aTimeout;
    struct timeval * timeout = NULL;

    if(seconds != -1 && microseconds != -1){
        aTimeout.tv_sec = seconds;
        aTimeout.tv_usec = microseconds;
        timeout = &aTimeout;
    }

    FD_ZERO(&fdvar);
    FD_SET(socket_num, &fdvar);

    if(select(socket_num+1, (fd_set *) &fdvar, NULL, NULL, timeout) < 0){
        perror("select");
        exit(-1);
    }

    if(FD_ISSET(socket_num, &fdvar)){
        return 1;
    }
    else{
        return 0;
    }
}

int safeSendto(uint8_t * packet, uint32_t len, Connection * to){
    int send_len = 0;

    if((send_len = sendtoErr(to->sk_num, packet, len, 0, (struct sockaddr *) &(to->remote), to->len)) < 0){
        perror("in safeSendto, sendto() call");
        exit(-1);
    }

    return send_len;
}

int safeRecvfrom(int recv_sk_num, uint8_t * packet, int len, Connection * from){
    int recv_len = 0;
    from->len = sizeof(struct sockaddr_in6);

    if((recv_len = recvfrom(recv_sk_num, packet, len, 0, (struct sockaddr *) &(from->remote), &from->len)) < 0){
        perror("in safeRecvfrom, recvfrom() call");
        exit(-1);
    }

    return recv_len;
}

void printIPv6Info(struct sockaddr_in6 * client){
    char ipString[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &client->sin6_addr, ipString, sizeof(ipString));
    printf("IP: %s Port: %d\n", ipString, ntohs(client->sin6_port));
}