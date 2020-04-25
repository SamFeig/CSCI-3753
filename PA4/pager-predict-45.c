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

void pageit(Pentry q[MAXPROCESSES]) { 

    /* 
    Globally calculated transition matrix for the programs that we were given.
    This was calculated by modifying the predictive LRU to count the probability
    of a page being requested given the previous page which had been requested. Predictive LRU was
    run 10 times on random seeds, and results averaged to generate the matrix. 

    Only used when the page changes, as most often the same page was requested as was already paged in,
    and so can be ignored in the overall distribution of this matrix. (It would have skewed it)

    Each row of the matrix represents a page (ordered by number), and each column represents the probability
    that page is paged in next given the current page.

    Initially, this predictive implementation calculated this matrix each time pageit() was called,
    but to improve performance, this value was made static, as it does not change much between
    runs of the simulator.
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


    for(proc = 0; proc< MAXPROCESSES; proc++) {
        // If process is active, implement predictive paging
        if(q[proc].active) {
            // Page the program counter needs
            pc = q[proc].pc;
            page = pc/PAGESIZE;

            // Set the current page to be swapped in
            swap[page] = 1;

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
                                // overall compute ratio, so this is as far forward
                                // as the algorithm will try to predict
                            } 
                        }
                    }
                }
            }

            // Page out any pages which have been determined to be unneeded
            // in the following few page operations
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(!swap[i] && q[proc].pages[i]) {
                    pageout(proc, i);
                }
            }

            // If the current page isn't already paged in,
            // page it in first since it is definitely needed
            if(!q[proc].pages[page]) {
                pagein(proc, page);
            }

            // Page in the remaining pages which were determined to be needed
            // in the following few page operations
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(swap[i] && !q[proc].pages[i]) {
                    pagein(proc, i);
                }
                swap[i] = 0;
            }         
        } else {
            /*
            If the process is not active, page out all pages related to that process
            (This may not be entirely necessary, as this seems to only marginally
            increase the overall compute ratio).
            */
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(q[proc].pages[i]) {
                    pageout(proc, i);
                }
            }
        }
    }
} 