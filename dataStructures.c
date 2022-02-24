#include "dataStructures.h"

void initializePageTable(pageTable pT[], int num) {
    int i = 0;
    while (i < num) {
        pT[i].pNum = -1;
        pT[i].rwBit = -1;
        pT[i].lru = 0;
        i++;
    }
}
