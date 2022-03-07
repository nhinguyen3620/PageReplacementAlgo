#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"
#include "policies.h"

void fifo(char *traceFile, int nFrames, char *mode) {
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
            //If it is, it's a Hit. Do nothing.
            for (int i = 0; i < nFrames; i++) {
                if (pT[i].pNum == currPage) {
                    if (bitType == 'W') pT[i].rwBit = 1;
                    if (strcmp(mode, "debug") == 0) printf("Page Hit!\n");
                    inPageTable = 1;
                    break;
                }
            }

            //Page is not in the page table
            if (inPageTable == 0) {
                //check if there is any available space
                int j = 0;
                while (j < nFrames) {
                    if (pT[j].pNum == -1) break;
                    j++;
                }
                
                //if there is no space, eliminate first page
                if (j == nFrames) {
                    if (pT[0].rwBit == 1) {
                        numWrite++;
                        if (strcmp(mode, "debug") == 0)
                            printf("Writing to disk!\n");
                    }
                    for (j = 0; j < nFrames - 1; j++) {
                        pT[j].pNum = pT[j + 1].pNum;
                        pT[j].rwBit = pT[j + 1].rwBit;
                    }
                    if (bitType == 'R') {
                        pT[nFrames-1].rwBit = 0;
                        if (strcmp(mode, "debug") == 0) printf("Reading from disk\n");
                    }
                    pT[nFrames - 1].rwBit = -1;
                }
                pT[j].pNum = currPage;
                if (bitType == 'W') pT[j].rwBit = 1;
                if(strcmp(mode, "debug") == 0) 
                    printf("Virtual page number %d is in frame %d\n", currPage, j);
                numRead++;
            }

        } 
    } 
    
    printf("Total memory frames: %d\n", nFrames);
    printf("Events in trace: %d\n", traceEvents);
    printf("Total disk reads: %d\n", numRead);
    printf("Total disk writes: %d\n", numWrite);
    printf("Hit: %.2f%%\n", (1 - (numRead/(double)traceEvents))*100);

}
