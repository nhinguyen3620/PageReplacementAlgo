#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "policies.h"

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        printf("Invalid arguments!");
    }

    char *traceFile, *policy, *mode;
    int nFrames, p;

    traceFile = argc[1];
    nFrames = atoi(argv[2]);
    policy = argc[3];

    if (argc == 5) {
        mode = argc[4];
        if (strcmp (policy, "fifo") == 0)
            fifo(traceFile, nFrames, mode);
        else if (strcmp(policy, "lru") == 0)
            lru(traceFile, nFrames, mode);
        else
            printf("Invalid argument!");
    }
        
    if (argc == 6) {
        p = atoi(argc[4]);
        mode = argc[5];
        if (strcmp(policy, "vms") == 0)
            vms(traceFile, nFrames, p, mode);
        else 
            printf("Invalid argument!");
    }
    return 0;

}