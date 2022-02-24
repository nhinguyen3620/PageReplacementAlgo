Files necessary to compile the program:
- memsim.c : main interface of the simulator, it takes user input in two forms
- fifo.c: file that implements first-in first-out replacement policy
- lru.c: file that implements least recently used policy
- vms.c: file that implements segmented-fifo polciy
- dataStructures.c: file that initializes the data structures necessary to run the program such as the page table

In addition to that the folder must also include policies.h, and dataStructures.h. The first one declares the policies previously mentioned to be used by the simulator, and the second one makes the dataStructures accessible to all the policies. Also, make sure to include any trace file to be tested in the same directory as well.

To compile the program:

1. You can use command "make" and it will compile all the files for you which creates memsim executable.
2. alternatively, you can also try "gcc -o memsim memsim.c fifo.c lru.c vms.c dataStructures.c", to compile the program.

To run the program:

The program can be run with two types of input format
1. For fifo and lru use the following format:
./memsim tracefile nFrames policy quiet/debug

2. For segmented fifo use the following format:
./memsim tracefile nframes policy percentage quiet/debug

where:
-policy: fifo, lru or vms
-percentage: 1-100
-tracefile: name of the tracefile
-nFrames: number of frames to use
- quiet/debug: quiet will just print the stats while debug will print every event that happens

