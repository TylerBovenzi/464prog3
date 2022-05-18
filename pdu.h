//
// Created by tyler on 5/11/2022.
//

#ifndef UDP_PDU_H
#define UDP_PDU_H

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int payloadLen);
void outputPDU(uint8_t * aPDU, int pduLength);

#endif //UDP_PDU_H


