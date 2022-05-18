#include <stdint-gcc.h>
#include <malloc.h>

struct frame{
    uint8_t valid;
    uint16_t seqNum;
    uint16_t pduSize;
    uint8_t *pdu;
};

struct window{
    struct frame *frames;
    uint16_t availableSize;
    uint16_t usedSize;
    uint16_t lower;
    uint16_t upper;
    uint16_t current;
    uint8_t open;
};

struct window* init( int winSize ){
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

uint8_t enqueue(struct window* win){
    if(win->usedSize > win->availableSize){
        return 0;
    }

    
    return 1;
}