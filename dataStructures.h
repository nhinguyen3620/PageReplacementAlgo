#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

typedef struct pageTable {
    int pNum;
    int leastUse;
    int rwBit;
} pageTable;

void initializePageTable(pageTable pT[], int num);
#endif