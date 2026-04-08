// #ifndef ALGORITHMS_H
// #define ALGORITHMS_H

// #define MAX_FRAMES 10
// #define MAX_PAGES 100

// typedef struct {
//     int step;
//     int page;
//     int frames[MAX_FRAMES];
//     int frame_count;
//     int is_fault;
//     int replaced;
// } Step;

// typedef struct {
//     char algorithm[16];
//     Step steps[MAX_PAGES];
//     int total_steps;
//     int hits;
//     int faults;
//     float hit_rate;
// } Result;

// Result run_fifo(int *ref, int n, int frames);
// Result run_lru(int *ref, int n, int frames);
// Result run_optimal(int *ref, int n, int frames);

// #endif


#ifndef ALGORITHMS_H 
#define ALGORITHMS_H 
 
/* ============================================================ 
 * algorithms.h  —  Module 1: Algorithm Engine 
 * Project    : Page Replacement Simulator (CA2) 
 * Author     : Member 1 
 * Description: Data structures and function declarations 
 * ============================================================ */ 
 
#define MAX_FRAMES  10      /* Maximum frames supported      */ 
#define MAX_PAGES   100     /* Maximum reference string size */ 
 
/* One step in the simulation */ 
typedef struct { 
    int  step;                      /* Step number (1-based)         */ 
    int  page;                      /* Page referenced in this step  */ 
    int  frames[MAX_FRAMES];        /* Frame contents after this step */ 
    int  frame_count;               /* How many frames are filled    */ 
    int  is_fault;                  /* 1 = page fault, 0 = page hit  */ 
    int  replaced;                  /* Page that was evicted (-1=none)*/ 
} Step; 
 
/* Full result from one algorithm run */ 
typedef struct { 
    char  algorithm[16];            /* "FIFO", "LRU", "Optimal"      */ 
    Step  steps[MAX_PAGES];         /* Array of all steps            */ 
    int   total_steps;              /* Total number of steps         */ 
    int   hits;                     /* Total page hits               */ 
    int   faults;                   /* Total page faults             */ 
    float hit_rate;                 /* hits/total * 100              */ 
} Result; 
 
/* Function declarations — implemented in fifo.c, lru.c, optimal.c */ 
Result run_fifo   (int *ref, int n, int frames); 
Result run_lru    (int *ref, int n, int frames); 
Result run_optimal(int *ref, int n, int frames); 
 
#endif /* ALGORITHMS_H */ 