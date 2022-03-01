#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"
#include "policies.h"

//udpate the read write bit
void updateRWBit(pageTable *pTable, char bitType);
//update the LRU tracker
void updateLRU(pageTable *pTable, int nFrames, int evicted);

void vms(char *traceFile, int nFrames, int p, char *mode) {
     FILE *file = fopen(traceFile, "r");

    //address
    unsigned int memoryAddr;
    //track the event, read, and write
    int eCounts = 0; 
    int rCounts = 0;
    int wCounts = 0;
    char bitType;
    //init the primary and secondary buffer 
    int nSecondaryBuffer =  (nFrames * p) / 100;
    int npBuffer = nFrames - nSecondaryBuffer;
    //Primary buffer 
    pageTable *pBuffer = malloc(npBuffer * sizeof(pageTable));
    initializePageTable(pBuffer, npBuffer);
    //secondary buffer 
    pageTable *sBuffer = malloc(nSecondaryBuffer * sizeof(pageTable));
    initializePageTable(sBuffer, nSecondaryBuffer);

    //fifo
    if(p == 0)
    {
        fifo(traceFile, nFrames, mode);
    }
    //lru
    else if (p == 100)
    {
        lru(traceFile,nFrames,mode);
    }
    //seg - fifo
    else
    {
        if (file != NULL) 
        {
            while (fscanf(file, "%x %c", &memoryAddr, &bitType) != EOF) {
                eCounts++; 
                unsigned pageNum = memoryAddr >> 12;
                int evictedPage_pos;
                int inPrimaryBuffer = 0;
                int inSecondaryBuffer = 0;
                int evicted = 0;
                pageTable evictedPage;
                int i;
                //loop to see if page in the p buffer, if not check the s buffer
                for (i = 0; i < npBuffer; i++)
                {
                    //PAGE IN THE PRIMARY BUFFER
                    if (pBuffer[i].pNum == pageNum) 
                    { 
                        inPrimaryBuffer = 1; 
                        updateLRU(sBuffer, nSecondaryBuffer, -1);  
                        updateRWBit(&pBuffer[i], bitType);
                        if (strcmp(mode, "debug") == 0) printf("page number: %d  hit in primary buffer\n",pageNum);
                        break;
                    }
                }

                //PAGE NOT IN THE P BUFFER
                if (inPrimaryBuffer  == 0) 
                {
                    int i;
                    for (i = 0; i < nSecondaryBuffer; i++) {
                        //PAGE IN SECONDARY BUFFER
                        if (sBuffer[i].pNum == pageNum) 
                        {   updateRWBit(&sBuffer[i], bitType);     
                            inSecondaryBuffer = 1; 
                            evictedPage_pos = i;                 
                            if (strcmp(mode, "debug") == 0) 
                                printf("page number: %d hit in secondary buffer\n",pageNum);
                            break;
                        }
                } 
                for (i = 0; i < npBuffer; i++){
                    if (pBuffer[i].pNum == -1) 
                    break;
                }
                //EVICT PAGE HANDLER - THERE NO SPACE IN primary buffer then ready to evict the first page in p buffer
                //then shift the p buffer using fifo
                if (i == npBuffer) {
                    //evict the first page
                    evictedPage.pNum = pBuffer[0].pNum;
                    evictedPage.rwBit = pBuffer[0].rwBit;
                    for (i = 0; i < npBuffer - 1; i++) {
                    pBuffer[i].pNum = pBuffer[i + 1].pNum;
                    pBuffer[i].rwBit = pBuffer[i + 1].rwBit;
                    }
                    pBuffer[npBuffer - 1].rwBit= -1;  
                    evicted = 1;

                    if (strcmp(mode, "debug") == 0) 
                        printf("page number: %d - page fault, evict the first page in primary buffer to secondary buffer\n",pageNum);
                } //IF page in the s buffer
                if (inSecondaryBuffer == 1) { 
                    pBuffer[i].pNum = sBuffer[evictedPage_pos].pNum;
                    pBuffer[i].rwBit = sBuffer[evictedPage_pos].rwBit;
                    updateRWBit(&pBuffer[i], bitType);

                    //DELETE EVICTED PAGE 
                    sBuffer[evictedPage_pos].pNum = -1;
                    sBuffer[evictedPage_pos].rwBit = -1;
                    sBuffer[evictedPage_pos].leastUse = 0;

                    if (strcmp(mode, "debug") == 0) printf("page number: %d in the secondary buffer. Moving page to front of primary buffer\n",pageNum);
                } 
                //PAGE IN SECONDARY BUFFER - HANDLE THE EVICTED PAGE
                else if (inSecondaryBuffer == 0) { 
                    //page fault
                    rCounts++;                
                    pBuffer[i].pNum = pageNum;
                    updateRWBit(&pBuffer[i], bitType);
                    if (strcmp(mode, "debug") == 0) printf("page number: %d - page fault: neither in primary buffer and secondary buffer\n", pageNum);
                }
                }
                //handling the evicted page
                //if there is space in s buffer then added in, else evict the page with biggest lru 
                    //if there is page evicted: from p buffer or s buffer, reset the old position
                    if (evicted == 1) {
                    int i;
                    int biggestLRU_pos;
                    int biggestLRU = 0;
                    for (i = 0; i < nSecondaryBuffer; i++){
                        if (sBuffer[i].pNum == -1)
                            break;
                        else if (sBuffer[i].leastUse > biggestLRU) {
                            biggestLRU_pos = i;
                            biggestLRU = sBuffer[i].leastUse;    
                    }
                    }
                    //if s buffer is full then evict from s buffer
                    if (i == nSecondaryBuffer) {
                        if (sBuffer[biggestLRU_pos].rwBit == 1) {
                            wCounts++; 
                        if (strcmp(mode, "debug") == 0) printf("saved to disk\n");
                        }
                        //clear the old space in secondary buffer
                        i = biggestLRU_pos;
                        sBuffer[biggestLRU_pos].rwBit = -1;
                        sBuffer[biggestLRU_pos].leastUse = 0;
                    } //store p buffer evicted page to s buffer
                    sBuffer[i].pNum = evictedPage.pNum;
                    sBuffer[i].rwBit = evictedPage.rwBit;
                    updateLRU(sBuffer, nSecondaryBuffer, i); 
                    if (strcmp(mode, "debug") == 0) 
                        printf("Add evicted page %d from primary buffer to the secondary buffer\n", evictedPage.pNum);
                }
            }
            free(pBuffer);
            free(sBuffer);
            printf("Total memory frames: %d\n", nFrames);
            printf("Events in trace: %d\n", eCounts);
            printf("Total disk reads: %d\n", rCounts);
            printf("Total disk writes: %d\n", wCounts);
            printf("Hit: %.2f%%\n", (1 - (rCounts/(double)eCounts))*100);
        }
            
    }
    
}

void updateRWBit(pageTable *pTable, char bitType) {
  if (pTable->rwBit == -1)
  {{
    if(bitType == 'W')
        pTable->rwBit = 1;
    else pTable->rwBit = 0;
    }
  }
  else if (bitType == 'W')
    pTable->rwBit = 1;
}

void updateLRU(pageTable *pTable, int nFrames, int evicted) {
  for (int i = 0; i < nFrames; i++)
    if (i == evicted)
      pTable[i].leastUse = 0;
    else if (pTable[i].pNum != -1)
      pTable[i].leastUse++;
}



