#include "window.h"

#define size 4

int main ( int argc, char *argv[]  )
{
    struct window* myWindow = winInit(size);
    printMetaData(myWindow);
    printWindow(myWindow);
    while(1){

    }
}