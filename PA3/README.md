### Programming Assignment 3:
##### CSCI 3753 Operating Systems Spring 2020
##### Sam Feig samuel.feig@colorado.edu

### Building
To build the program, navigate to the directory containing `multi-lookup.c`,`multi-lookup.h`, `util.c`, `util.h`, `queue.c`, `queue.h` and `Makefile` then in a command line run:
```
make
```
To clean up all installed and created files, run:
```
make clean
```

### Running
- To run the program with 8 requester threads and 5 resolver threads and all data files, run:
```
make run
```

- To run the program with custom arguments, use the following input format:
```
./multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ...]
``` 

- To run the program with valgrind using 8 requester threads and 5 resolver threads and all data files, run:
```
make valgrind
```

***NOTES:***
Make sure the filepaths specified for the requester and resolver log exist and that there is at least one valid datafile path. In addition, the program may not have more than 10 input files, 10 resolver threads, and 5 requester threads.