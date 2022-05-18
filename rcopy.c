// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>/home/tyler/Desktop
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "pdu.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "cpe464.h"

#define MAXBUF 80

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
void checkArgs(int argc, char * argv[], int *portNumber, double *errorRate);


int main (int argc, char *argv[])
 {


	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	double errorRate = 0;
    checkArgs(argc, argv, &portNumber, &errorRate);
    sendtoErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
	
	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
    uint32_t num = 0;
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0; 
	char buffer[MAXBUF+1];
	uint8_t pduBuf[1407];

	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = readFromStdin(buffer);

		printf("Sending: %s with len: %d\n", buffer,dataLen);

        int len = createPDU(pduBuf, num, 0, buffer, dataLen);
        outputPDU(pduBuf, len);
        num++;

		sendtoErr(socketNum, buffer, dataLen, 0, (struct sockaddr *) server, serverAddrLen);
		
		safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) server, &serverAddrLen);
		
		// print out bytes received
		ipString = ipAddressToString(server);
		printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), buffer);
	      
	}
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[],int *portNumber, double *errorRate)
{

    /* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: server error-rate host-name port-number \n", argv[0]);
		exit(1);
	}
	
	*portNumber = atoi(argv[3]);
    *errorRate = atof(argv[1]);

}





