#include <stdint-gcc.h>
#include <malloc.h>
#include <string.h>

struct frame{
    uint8_t valid;
    uint32_t seqNum;
    uint32_t pduSize;
    uint8_t pdu[1407];
};

struct window{
    struct frame* frames;
    uint16_t availableSize;
    uint16_t usedSize;
    uint32_t lower;
    uint32_t upper;
    uint32_t current;
    uint8_t open;
};

struct window* winInit( int winSize ){
    struct window *newWin;
    newWin = (struct window*)malloc(sizeof(struct window));

    newWin->frames          = (struct frame*)malloc(winSize * sizeof(struct frame));
    newWin->availableSize   = winSize;
    newWin->usedSize        = 0;

    newWin->lower           = 0;
    newWin->current         = 0;
    newWin->upper           = winSize;

    newWin->open = 1;

    return newWin;
};

void printMetaData(struct window* win){
    printf("Server Window - Window Size: %d, lower: %d, Upper: %d, Current: %d, Window open: %d\n",
           win->availableSize, win->lower, win->upper, win->current, win->open);
}

void printWindow(struct window* win){
    printf("Window size is: %d\n", win->availableSize);
    for(int i = 0; i<win->availableSize; i++){
        struct frame* currentFrame = &win->frames[i];
        printf("\t%d ", i);
        if(currentFrame->valid)
            printf("sequenceNumber: %d pduSize: %d\n", currentFrame->seqNum, currentFrame->pduSize);
        else
            printf("not valid\n");
    }
}


uint8_t enqueue(struct window* win, uint16_t seqNum, uint16_t pduSize, uint8_t *pdu){
    if(win->usedSize > win->availableSize){
        return 0;
    }

    struct frame* newFrame = &win->frames[seqNum % win->availableSize];
    newFrame->valid     = 1;
    newFrame->seqNum    = seqNum;
    newFrame->pduSize    = pduSize;
    memcpy(&newFrame->pdu[0], &pdu[0], pduSize);

    (win->usedSize)++;
    (win->current)++;

    if(win->usedSize == win->availableSize){
        win->open=0;
    }
    return 1;
}

uint8_t isOpen(struct window* win){
    return win->open;
}

uint32_t getCurrent(struct window* win){
    return win->current;
}

uint8_t processRR(struct window* win, uint16_t seqNum){
    if(seqNum > win->current || seqNum < win->current){
        return 0;
    }
    uint16_t cursor = win->lower;
    while(cursor < seqNum){
        (win->upper)++;
        (win->lower)++;
        struct frame* oldFrame = &win->frames[cursor % win->availableSize];
        oldFrame->valid = 0;
        (win->usedSize)--;
        cursor++;
    }
    win->open = 1;
    return 1;
}


uint8_t* getPDU(uint8_t* dest, struct window* win, uint32_t seqNum, uint32_t * pduSize){
    uint16_t cursor = win->lower;
    while(cursor < seqNum){
        cursor++;
    }
    *pduSize = (&win->frames[cursor])->pduSize;
    return (win->frames[cursor]).pdu;
}



