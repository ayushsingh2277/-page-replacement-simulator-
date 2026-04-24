#ifndef DISPLAY_H
#define DISPLAY_H
/* ============================================================
* display.h — Module 2: Display & Reporter
* Author : Member 2
* ============================================================ */
#include "../module1_algorithms/algorithms.h"
/* Print step-by-step execution table for one algorithm */
void display_result(Result *r);
/* Print a side-by-side comparison of all 3 algorithms */
void display_comparison(Result *fifo, Result *lru, Result *optimal);
#endif /* DISPLAY_H */