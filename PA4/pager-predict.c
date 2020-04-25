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
 *  This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

#define SWAP_POINT 100 //Use static transition matrix for first n=100 iterations

void pageit(Pentry q[MAXPROCESSES]) { 

    static int initialized = 0;
    static int previous[MAXPROCESSES]; // Keep track of previous page paged in
    // Array of transition matrices
    static int predictCount[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    // Keep track of total page occurances for each process (used in calculating probabilities)
    static int rowSums[MAXPROCESSES][MAXPROCPAGES];
    static int tick = 1;

    /* initialize static vars on first run */
    if(!initialized){
        for(int proctmp=0; proctmp < MAXPROCESSES; proctmp++){
            previous[proctmp] = -1;
            for(int i = 0; i < MAXPROCPAGES; i++) {
                rowSums[proctmp][i] = 0;
                for(int j = 0; j < MAXPROCPAGES; j++) {
                    predictCount[proctmp][i][j] = 0;
                }
            }
        }
        initialized = 1;
    }

    /* 
    Globally calculated transition matrix for the programs that we were given.
    This was calculated by modifying the predictive LRU to count the probability
    of a page being requested given the previous page which had been requested. Predictive LRU was
    run 10 times on random seeds, and results averaged to generate the matrix. 

    Only used when the page changes, as most often the same page was requested as was already paged in,
    and so can be ignored in the overall distribution of this matrix. (It would have skewed it)

    Each row of the matrix represents a page (ordered by number), and each column represents the probability
    that page is paged in next given the current page.

    This transition matrix is only used for the first SWAP_POINT interations, after which point
    it replaced by a unique transition matrix for each process.
    */

    const float transition[MAXPROCPAGES][MAXPROCPAGES] = {
        {0,       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0.296,   0,     0,     0,     0.576, 0,     0,     0,     0,     0,     0.126, 0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0.599,   0,     0,     0,     0,     0,     0,     0,     0,     0.401, 0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0,     0,     0},
        {0.095,   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0.905, 0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,     0,     0},
        {0.06,    0,     0,     0,     0,     0,     0,     0,     0,     0.93,  0,     0,     0,     0,     0.005, 0,     0,     0,     0,     0},
        {1,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
        {0,       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0},
    };


    /* Local vars */
    int pc; // Program counter
    int proc; // Process number used in the loop below
    int page; // Current Page needed based on the program counter
    // Boolean array to determine which pages should be swapped in and out
    int swap[MAXPROCPAGES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 


    for(proc = 0; proc < MAXPROCESSES; proc++) {
        // If process is active, implement predictive paging
        if(q[proc].active) {
            // Page the program counter needs
            pc = q[proc].pc;
            page = pc/PAGESIZE;

            // Calculate/update each transition matrix for current process
            if(previous[proc] != -1 && previous[proc] != page) {
                rowSums[proc][previous[proc]] += 1;
                predictCount[proc][previous[proc]][page] += 1;
            }
            // Update previous value for next iteration
            previous[proc] = page;

            // Set the current page to be swapped in
            swap[page] = 1;

            // Once we're past the swap point, use the individual transition matrices
            // Now have enough data
            if(tick > SWAP_POINT && rowSums[proc][page]) {
                for(int i = 0; i < MAXPROCPAGES; i++) {
                    // Swap in any next page which has probability > 0 of occuring
                    if((double) predictCount[proc][page][i]/rowSums[proc][page]) {
                        swap[i] = 1;
                        if(rowSums[proc][i]) {
                            for(int j = 0; j < MAXPROCPAGES; j++) {
                                // Swap in any page which has a probability > 0 of occuring
                                // given that page i has occurred previously
                                if((double) predictCount[proc][i][j]/rowSums[proc][i]) {
                                    swap[j] = 1;
                                }
                            }
                        }
                    }
                }
            } 
            // For the first few iterations, use the general transition matrix
            else {
                for(int i = 0; i < MAXPROCPAGES; i++) {
                    // Given any probability that the next page is 'i', swap it in
                    if(transition[page][i]) {
                        swap[i] = 1;
                        // Given a high probability of the next page being 'i' (> .9),
                        // check all the possible values for the next page after 'i'.
                        if(transition[page][i] > .9) {
                            for(int j = 0; j < MAXPROCPAGES; j++) {
                                // If the cumulative probability of 'j' being the page
                                // after 'i' is > .025, swap in 'j'.
                                if(transition[i][j]*transition[page][i] > .025) {
                                    swap[j] = 1;
                                    // Going another step forward seems to hinder the 
                                    // overall ratio, so did not try to predict farther ahead
                                } 
                            }
                        }
                    }
                }   
            }

            // Page out any pages which have been determined to be unneeded
            // in the following few page operations by probabilities
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(!swap[i] && q[proc].pages[i]) {
                    pageout(proc, i);
                }
            }

            // If the current page isn't already paged in,
            // page it in first since it is needed
            if(!q[proc].pages[page]) {
                pagein(proc, page);
            }

            // Page in the remaining pages which were determined to be needed
            // in the following few page operations by probabilities
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(swap[i] && !q[proc].pages[i]) {
                    pagein(proc, i);
                }
                swap[i] = 0;
            }         
        } else {
            /*
            If the process is not active, page out all pages related to that process
            (This may not be necessary, as it only marginally increases the ratio 
            but lets do it anyway
            */
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(q[proc].pages[i]) {
                    pageout(proc, i);
                }
            }
        }
    }
    tick++;
} 