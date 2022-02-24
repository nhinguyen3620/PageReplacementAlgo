#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

typedef struct pageTable {
    int pNum;
    int lru;
    int rwBit;
} pageTable;

void initializePageTable(pageTable pT[], int num);
#endif