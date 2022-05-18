//
// Created by tyler on 5/18/2022.
//

#ifndef UDP_WINDOW_H
#define UDP_WINDOW_H

#include <stdint-gcc.h>
#include <malloc.h>
#include <string.h>

struct frame;

struct window;

struct window* winInit( int winSize );

void printMetaData(struct window* win);

void printWindow(struct window* win);


uint8_t enqueue(struct window* win, uint16_t seqNum, uint16_t pduSize, uint8_t *pdu);

uint8_t processRR(struct window* win, uint16_t seqNum);

uint16_t getPDU(uint8_t* dest, struct window* win, uint16_t seqNum);

#endif //UDP_WINDOW_H
