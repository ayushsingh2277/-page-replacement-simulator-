#include<stdio.h>
#include <string.h>
#include "display.h"
/* ============================================================
* display.c — Module 2: Display & Reporter
* ============================================================ */
/* ANSI color codes for terminal output */
#define GREEN "[0;32m"
#define RED "[0;31m"
#define CYAN "[0;36m"
#define YELLOW "[1;33m"
#define BOLD "[1m"
#define RESET "[0m"
/* ── DISPLAY STEP-BY-STEP TABLE ── */
void display_result(Result *r) {
 printf("
%s========== %s SIMULATION ==========%s
",
 CYAN, r->algorithm, RESET);
 /* Print column headers */
 printf("%s%-6s %-6s", BOLD, "Step", "Page");
 for (int f = 1; f <= r->steps[0].frame_count; f++)
 printf("Frame%-2d ", f);
 printf("%-8s Replaced%s
", "Status", RESET);
 /* Separator line */
 printf("-------------------------------------------"
 "-------------------
");
 /* Print each step */
 for (int i = 0; i < r->total_steps; i++) {
 Step *s = &r->steps[i];
 printf("%-6d %-6d", s->step, s->page);
 /* Print frame contents */
 for (int f = 0; f < s->frame_count; f++) {
 if (s->frames[f] == -1)
 printf(" - ");
 else
 printf(" %-5d ", s->frames[f]);
 }
 /* Print status — GREEN for HIT, RED for FAULT */
 if (s->is_fault)
 printf("%sFAULT %s", RED, RESET);
 else
 printf("%sHIT %s", GREEN, RESET);
 /* Print replaced page */
 if (s->replaced == -1)
 printf(" -
");
 else
 printf(" %d
", s->replaced);
 }
 /* Print summary */
 printf("
%s--- SUMMARY ---%s
", YELLOW, RESET);
 printf(" Algorithm : %s%s%s
", BOLD, r->algorithm, RESET);
 printf(" Total Refs: %d
", r->total_steps);
 printf(" %sPage Hits : %d%s
", GREEN, r->hits, RESET);
 printf(" %sPage Faults: %d%s
", RED, r->faults, RESET);
 printf(" Hit Rate : %.1f%%
", r->hit_rate);
}
/* ── COMPARISON TABLE ── */
void display_comparison(Result *fifo, Result *lru, Result *optimal) {
 printf("
%s========================================%s
", CYAN, RESET);
 printf("%s ALGORITHM COMPARISON TABLE %s
", BOLD, RESET);
 printf("%s========================================%s
", CYAN, RESET);
 printf("
%s%-15s %-10s %-10s %-10s%s
",
 BOLD, "Metric", "FIFO", "LRU", "Optimal", RESET);
 printf("%-15s %-10s %-10s %-10s
",
 "---------------", "----------", "----------", "----------");
 printf("%-15s %-10d %-10d %-10d
",
 "Page Faults",
 fifo->faults, lru->faults, optimal->faults);
 printf("%-15s %-10d %-10d %-10d
",
 "Page Hits",
 fifo->hits, lru->hits, optimal->hits);
 printf("%-15s %-9.1f%% %-9.1f%% %-9.1f%%
",
 "Hit Rate",
 fifo->hit_rate, lru->hit_rate, optimal->hit_rate);
 /* Winner analysis */
 printf("
%s--- ANALYSIS ---%s
", YELLOW, RESET);
 /* Find best (fewest faults) */
 if (optimal->faults <= lru->faults && optimal->faults <= fifo->faults)
 printf(" %sBest Algorithm : Optimal (%d faults)%s
",
 GREEN, optimal->faults, RESET);
 else if (lru->faults <= fifo->faults)
 printf(" %sBest Algorithm : LRU (%d faults)%s
",
 GREEN, lru->faults, RESET);
 else
 printf(" %sBest Algorithm : FIFO (%d faults)%s
",
 GREEN, fifo->faults, RESET);
 printf(" LRU vs FIFO improvement: %d fewer faults
",
 fifo->faults - lru->faults);
 printf(" Optimal vs LRU gap : %d fewer faults
",
 lru->faults - optimal->faults);
}