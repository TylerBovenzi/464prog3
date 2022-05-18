/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pdu.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "cpe464.h"
#define MAXBUF 80

void processClient(int socketNum);
void checkArgs(int argc, char * argv[], int *portNumber, double *errorRate);

int main ( int argc, char *argv[]  )
{ 
	int socketNum = 0;
	int portNumber = 0;
    double errorRate = 0;
	checkArgs(argc, argv, &portNumber, &errorRate);

    sendtoErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
    socketNum = udpServerSetup(portNumber);

    processClient(socketNum);

	close(socketNum);
	
	return 0;
}

void processClient(int socketNum)
{
	int dataLen = 0;
    uint16_t bufLen = 0;
    char buffer[MAXBUF + 1];
    char pdu[MAXBUF + 1];
    struct sockaddr_in6 client;
	int clientAddrLen = sizeof(client);
    int num = 0;
    while(1) {

        bufLen = safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);

        printf("Received message from client with ");
        printIPInfo(&client);
        printf(" Len: %d \'%s\'\n", dataLen, buffer);
        sprintf(buffer, "bytes: %d", dataLen);
        dataLen = createPDU(pdu, num, 3, buffer, bufLen);
        num++;
        sendtoErr(socketNum, pdu, dataLen, 0, (struct sockaddr *) &client, clientAddrLen);
    }
}

void checkArgs(int argc, char * argv[], int *portNumber, double *errorRate)
{

    if (argc == 1)
    {
        fprintf(stderr, "Usage %s error-rate [optional port number]\n", argv[0]);
        exit(-1);
    }

	if (argc > 3)
	{
		fprintf(stderr, "Usage %s error-rate [optional port number]\n", argv[0]);
		exit(-1);
	}

    *errorRate = atof(argv[1]);

	if (argc == 3)
	{
		*portNumber = atoi(argv[2]);
	}

	return portNumber;
}


