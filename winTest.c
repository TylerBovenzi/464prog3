#include "window.h"
#include <stdio.h>

#define size 4

int main ( int argc, char *argv[]  )
{
    struct window* myWindow = winInit(size);
    int index = 0;
    int response;
    while(1){
        if(isOpen(myWindow)){
            printMetaData(myWindow);
            enqueue(myWindow, index, 20+index, "                          ");
            index++;
        }else {
            printWindow(myWindow);
            printMetaData(myWindow);
            printf("\nEnter Frame to RR: ");
            scanf("%d", &response);
            processRR(myWindow, response);
            printf("\n");
            printWindow(myWindow);
        }

    }
}