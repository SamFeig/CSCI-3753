/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int previous[MAXPROCESSES];
    static int predictCount[MAXPROCPAGES][MAXPROCPAGES];
    static int rowSums[MAXPROCPAGES];

    /* Local vars */
    int pc;
    int proc;
    int page;

    /* initialize static vars on first run */
    if(!initialized){
        for(int i = 0; i < MAXPROCPAGES; i++) {
            rowSums[i] = 0;
            previous[i] = -1;

            for(int j = 0; j < MAXPROCPAGES; j++) {
                timestamps[i][j] = 0;
                predictCount[i][j] = 0;
            }
            previous[i] = -1;
        }
        initialized = 1;
    }

    for(proc = 0; proc < MAXPROCESSES; proc++) {
        // Check if the process is active
        if(q[proc].active) {
            // Page the program counter needs
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            
            //Predict
            if(previous[proc] != -1 && previous[proc] != page) {
                rowSums[previous[proc]] += 1;
                predictCount[previous[proc]][page] += 1;
            }
            previous[proc] = page;

            // Boolean array of whether we will need each page or not
            int swap[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            swap[page] = 1;

            //More than 1 in the row
            if(rowSums[page]) {
                for(int i = 0; i < MAXPROCPAGES; i++) {
                    //Check probabiklity and swap if > 0
                    if((double) predictCount[page][i]/rowSums[page] > 0) {
                        swap[i] = 1;
                    }
                    //Check if probability > 0.8 and sum is more than 1 (look ahead to second proc)
                    if((double) predictCount[page][i]/rowSums[page] > .8 && rowSums[i]) {
                        for(int j = 0; j < MAXPROCPAGES; j++) {
                            if((double) predictCount[i][j]/rowSums[i] > 0) {
                                swap[j] = 1;
                            }
                        }
                    }
                }
            }

            //Pageout unmarked procs
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(!swap[i]) {
                    pageout(proc, i);
                }
            }

            //Pagein marked procs that arent already in
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(swap[i] && !q[proc].pages[i]) {
                    pagein(proc, i);
                }
            }         
        }
    }

    //Print data

    // if(tick > 3000000 && tick < 3000020) {
    //     // printf("%d\n", tick);
    //     int rowsums[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //     for(int i = 0; i < MAXPROCPAGES; i++) {
    //         for(int j = 0; j < MAXPROCPAGES; j++) {
    //             rowsums[i] += predictCount[i][j];
    //         }
    //     }
    //     for(int i = 0; i < MAXPROCPAGES; i++) {
    //         for(int j = 0; j < MAXPROCPAGES; j++) {
    //             if(rowsums[i] > 0) {
    //                 printf("%.2f\t", (double)predictCount[i][j]/rowsums[i]);
    //             } else {
    //                 printf("%d\t", 0);
    //             }
    //         }
    //         printf("\n");
    //     }
    // }
} 