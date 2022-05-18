#include <stdint-gcc.h>
#include <malloc.h>
#include <netinet/in.h>
#include <string.h>
#include "libcpe464/checksum.h"

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int payloadLen){
    uint32_t sequenceNet = htonl(sequenceNumber);
    memcpy(&pduBuffer[0], &sequenceNet, 4);
    pduBuffer[4] = 0;
    pduBuffer[5] = 0;
    pduBuffer[6] = flag;
    memcpy(&pduBuffer[7], &payload[0], payloadLen);
    uint16_t cksm = in_cksum(&pduBuffer[0],payloadLen+7);
    memcpy(&pduBuffer[4], &cksm, 2);
    return payloadLen+7;
}

void outputPDU(uint8_t * aPDU, int pduLength){
    uint32_t sequenceNumber;
    memcpy(&sequenceNumber, &aPDU[0], 4);
    sequenceNumber = ntohl(sequenceNumber);
    printf("\nPDU \t#%d\n", sequenceNumber);

    uint16_t cksm = in_cksum(&aPDU[0], pduLength);
    uint16_t cksm2;
    memcpy(&cksm2, &aPDU[4], 2);
    printf("CheckSum: %s \t(%d)\n", !cksm?"Valid":"Invalid", cksm2);

    uint8_t flag = aPDU[6];
    printf("Flag: %d\n", flag);

    char payloadBuf[1401];
    int payloadLength = pduLength-7;
    memcpy(&payloadBuf, &aPDU[7], payloadLength);

    printf("Payload: %s\n", payloadBuf);
    printf("Payload Length: %d\n\n", payloadLength);
};