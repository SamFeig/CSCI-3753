### Programming Assignment 4:
##### CSCI 3753 Operating Systems Spring 2020
##### Sam Feig samuel.feig@colorado.edu

### Files
```
Makefile - GNU makefile to build all relevant code
pager-basic.c - Basic paging strategy implementation that runs one process at a time.
pager-lru.c - LRU paging strategy implementation
pager-lru-probabilities.c - Modified LRU with probabilities (Used to calculate transition matrix for predictive implementation)
pager-predict.c - Predictive paging strategy implementation
api-test.c - A pageit() implmentation that tests that simulator state changes
simulator.c - Core simualtor code (look but don't touch)
simulator.h - Exported functions and structs for use with simulator
programs.c - Defines test "programs" for simulator to run
pgm*.pseudo - Pseudo code of test programs from which programs.c was generated.
```

### Executables
```
test-* - Runs simulator using "programs" defined in programs.c
         and paging strategy defined in pager-*.c.
         Includes various run-time options. Run with '-help' for details.
test-api - Runs a test of the simulator state changes
```

### Building
To build all parts of this project run:
```make```

Clean:
```make clean```

View test options:
```./test-* -help```

Run pager-basic test:
```./test-basic```

Run pager-lru test:
```./test-lru```

Run pager-lru-probabilities test and print matrix:
```./test-lru-print```

Run pager-predict test:
```./test-predict```

Run API test:
```./test-api```
