#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "policies.h"
#include "dataStructures.h"

int main(int argc, char *argv[]) {
    printf("debug");
    if (argc < 5 || argc > 6) {
        printf("Invalid arguments!");
    }

    char *traceFile, *policy, *mode;
    int nFrames, p;

    traceFile = argv[1];
    nFrames = atoi(argv[2]);
    policy = argv[3];
    printf("debug");
    if (argc == 5) {
        mode = argv[4];
        if (strcmp (policy, "fifo") == 0)
            fifo(traceFile, nFrames, mode);
        else if (strcmp(policy, "lru") == 0)
            lru(traceFile, nFrames, mode);
        else
            printf("Invalid argument!");
    }
        
    if (argc == 6) {
        p = atoi(argv[4]);
        mode = argv[5];
        if (strcmp(policy, "vms") == 0)
            vms(traceFile, nFrames, p, mode);
        else 
            printf("Invalid argument!");
    }
    return 0;

}

