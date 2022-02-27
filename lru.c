#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"
#include "policies.h"

void lru(char *traceFile, int nFrames, char *mode) {
    FILE *file = fopen(traceFile, "r");
    unsigned int address;
    int traceEvents = 0, numRead = 0, numWrite = 0;
    char bitType;

    pageTable *pT = malloc(nFrames * sizeof(pageTable));
    initializePageTable(pT, nFrames);

    //read from file
    if (file != NULL) {
        while(fscanf(file, "%x %c", &address, &bitType) != EOF) {
            traceEvents++;
            unsigned int currPage = address >> 12;
            int inPageTable = 0;
            //First, loop through page table to see if page is alr in the page table
            for (int i = 0; i < nFrames; i++) {
                //increment leastUse for page that is allocated
                if (pT[i].pNum != -1) pT[i].leastUse++;
                //page is in the table
                if (pT[i].pNum == currPage) {
                    if (bitType == 'W') pT[i].rwBit = 1;
                    if (strcmp(mode, "debug") == 0) printf("Page Hit!\n");
                    inPageTable = 1;
                    pT[i].leastUse = 0;
                }
            }

            //page is not in page table
            if (inPageTable == 0) {
                int lruMax = 0, lruPos;
                //check if there is any available space
                int j = 0;
                while (j < nFrames) {
                    if (pT[j].pNum == -1) break;
                    if (pT[j].leastUse > lruMax) {
                        lruMax = pT[j].leastUse;
                        lruPos = j;
                    }
                    j++;
                }

                //if there is no available space, eliminate page that has lruMax
                if (j == nFrames) {
                    if (pT[lruPos].rwBit == 1) {
                        numWrite++;
                        if (strcmp(mode, "debug") == 0)
                            printf("Writing to disk!\n");
                    }

                    //reset the recently-eliminated page
                    pT[lruPos].rwBit = -1;
                    pT[lruPos].leastUse = 0;

                    j = lruPos;
                }
                numRead++;

                //update leastUse
                if (pT[j].pNum != -1)
                    pT[j].leastUse++;
                pT[j].leastUse = 0;
                
                pT[j].pNum = currPage;
                if (bitType == 'W') pT[j].rwBit = 1;
                if(strcmp(mode, "debug") == 0) 
                   printf("Virtual page number %d is allocated in frame %d\n", currPage, j);
                }
            }  
        }
        printf("Total memory frames: %d\n", nFrames);
        printf("Events in trace: %d\n", traceEvents);
        printf("Total disk reads: %d\n", numRead);
        printf("Total disk writes: %d\n", numWrite);
        printf("Hit: %.2f%%\n", (1 - (numRead/(double)traceEvents))*100);
    }
    

