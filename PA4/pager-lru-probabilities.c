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
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int previous[MAXPROCESSES];
    static int predictCount[MAXPROCPAGES][MAXPROCPAGES];

    /* Local vars */
    int pc;
    int proc;
    int page;
    

    /* initialize static vars on first run */
    if(!initialized){
        for(int i = 0; i < MAXPROCPAGES; i++) {
            previous[i] = -1;

            for(int j = 0; j < MAXPROCPAGES; j++) {
                timestamps[i][j] = 0;
                predictCount[i][j] = 0;
            }   
        }
        initialized = 1;
    }
    
    // Implement LRU Paging
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        // Check if the process is active
        if(q[proc].active) {
            // Page the program counter needs
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            timestamps[proc][page] = tick;
            
            if(previous[proc] != -1 && previous[proc] != page) {
                predictCount[previous[proc]][page] += 1;
            }
            previous[proc] = page;
            // Check if page is swapped out
            if(!q[proc].pages[page]) {
                // Try to swap in
                if(!pagein(proc, page)) {
                    // If swapping fails, need to find another page to swap out
                    int min = tick;
                    int choice;
                    // Find least recently used page for the current process which is currently paged in
                    for(int i = 0; i < q[proc].npages; i++) {
                        if(timestamps[proc][i] < min && q[proc].pages[i]) {
                            choice = i;
                            min = timestamps[proc][i];
                        }
                    }
                    // Pageout least recently used page
                    pageout(proc, choice);
                }
            }
        }
        // Advance time for next pageit iteration
        tick++;
    } //Do printing of transition matrix if finished
    if(tick > 3000000 && tick < 3000020) {
        // printf("%d\n", tick);
        int rowsums[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for(int i = 0; i < MAXPROCPAGES; i++) {
            for(int j = 0; j < MAXPROCPAGES; j++) {
                rowsums[i] += predictCount[i][j];
            }
        }
        for(int i = 0; i < MAXPROCPAGES; i++) {
            for(int j = 0; j < MAXPROCPAGES; j++) {
                if(rowsums[i] > 0) {
                    printf("%.2f\t", (double)predictCount[i][j]/rowsums[i]);
                } else {
                    printf("%d\t", 0);
                }
            }
            printf("\n");
        }
    }
}
