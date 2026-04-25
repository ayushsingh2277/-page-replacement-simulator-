/*
 * page_replacement.h
 * Header for Page Replacement Algorithm Simulator
 */

#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#define MAX_FRAMES   10
#define MAX_REF_LEN  100
#define EMPTY        -1

/* ── Data Structures ─────────────────────────────────────── */

/* Snapshot of one simulation step */
typedef struct {
    int page_ref;                   /* Page referenced at this step      */
    int frame_state[MAX_FRAMES];    /* Frame contents after this step    */
    int frame_count;                /* Number of frames in use           */
    int is_fault;                   /* 1 = page fault, 0 = hit           */
    int replaced_page;              /* Page evicted (EMPTY if none)      */
} StepInfo;

/* Complete simulation result */
typedef struct {
    StepInfo *steps;    /* Array of step snapshots (malloc'd) */
    int total_refs;     /* Length of the reference string     */
    int page_faults;    /* Total page faults                  */
    double hit_ratio;   /* Hit percentage (0–100)             */
} SimResult;

/* ── Algorithm Functions ─────────────────────────────────── */

SimResult simulate_fifo   (const int *ref_str, int ref_len, int frame_count);
SimResult simulate_lru    (const int *ref_str, int ref_len, int frame_count);
SimResult simulate_optimal(const int *ref_str, int ref_len, int frame_count);

/* ── Utility ─────────────────────────────────────────────── */

void free_result (SimResult *r);
void print_result(const SimResult *r, const char *algo_name);

#endif /* PAGE_REPLACEMENT_H */
