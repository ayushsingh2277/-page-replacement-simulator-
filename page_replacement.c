/*
 * ============================================================
 *  Page Replacement Algorithm Simulator - Core Logic (C)
 *  Algorithms: FIFO, LRU, Optimal
 *  Author: OS Project
 * ============================================================
 */

#include "page_replacement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── Utility ──────────────────────────────────────────────── */

static void init_frames(int *frames, int n) {
    for (int i = 0; i < n; i++) frames[i] = EMPTY;
}

static int find_in_frames(const int *frames, int n, int page) {
    for (int i = 0; i < n; i++)
        if (frames[i] == page) return i;
    return -1;
}

static int has_empty_frame(const int *frames, int n) {
    for (int i = 0; i < n; i++)
        if (frames[i] == EMPTY) return i;
    return -1;
}

/* ─── FIFO ──────────────────────────────────────────────────── */

SimResult simulate_fifo(const int *ref_str, int ref_len, int frame_count) {
    SimResult result;
    memset(&result, 0, sizeof(result));
    result.steps = (StepInfo *)malloc(ref_len * sizeof(StepInfo));
    result.total_refs = ref_len;

    int frames[MAX_FRAMES];
    init_frames(frames, frame_count);
    int queue[MAX_FRAMES];   /* circular queue tracking insertion order */
    init_frames(queue, frame_count);
    int queue_ptr = 0;       /* next victim pointer */
    int filled = 0;

    for (int i = 0; i < ref_len; i++) {
        int page = ref_str[i];
        StepInfo *s = &result.steps[i];
        s->page_ref = page;

        int hit = find_in_frames(frames, frame_count, page);
        if (hit != -1) {
            /* HIT */
            s->is_fault = 0;
            s->replaced_page = EMPTY;
        } else {
            /* FAULT */
            s->is_fault = 1;
            result.page_faults++;

            int empty = has_empty_frame(frames, frame_count);
            if (empty != -1) {
                frames[empty] = page;
                queue[empty] = page;
                filled++;
                s->replaced_page = EMPTY;
            } else {
                /* Evict FIFO victim */
                s->replaced_page = frames[queue_ptr];
                /* Find and replace that page in frames */
                int fi = find_in_frames(frames, frame_count, queue[queue_ptr]);
                frames[fi] = page;
                queue[queue_ptr] = page;
                queue_ptr = (queue_ptr + 1) % frame_count;
            }
        }

        /* Snapshot current frames */
        for (int f = 0; f < frame_count; f++)
            s->frame_state[f] = frames[f];
        s->frame_count = frame_count;
    }

    result.hit_ratio = (ref_len > 0)
        ? (double)(ref_len - result.page_faults) / ref_len * 100.0
        : 0.0;
    return result;
}

/* ─── LRU ───────────────────────────────────────────────────── */

SimResult simulate_lru(const int *ref_str, int ref_len, int frame_count) {
    SimResult result;
    memset(&result, 0, sizeof(result));
    result.steps = (StepInfo *)malloc(ref_len * sizeof(StepInfo));
    result.total_refs = ref_len;

    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES];   /* timestamp of last use per frame slot */
    init_frames(frames, frame_count);
    memset(last_used, 0, sizeof(last_used));

    for (int i = 0; i < ref_len; i++) {
        int page = ref_str[i];
        StepInfo *s = &result.steps[i];
        s->page_ref = page;

        int hit = find_in_frames(frames, frame_count, page);
        if (hit != -1) {
            s->is_fault = 0;
            s->replaced_page = EMPTY;
            last_used[hit] = i;
        } else {
            s->is_fault = 1;
            result.page_faults++;

            int empty = has_empty_frame(frames, frame_count);
            if (empty != -1) {
                frames[empty] = page;
                last_used[empty] = i;
                s->replaced_page = EMPTY;
            } else {
                /* Find LRU victim (minimum last_used) */
                int lru_idx = 0;
                for (int f = 1; f < frame_count; f++)
                    if (last_used[f] < last_used[lru_idx]) lru_idx = f;

                s->replaced_page = frames[lru_idx];
                frames[lru_idx] = page;
                last_used[lru_idx] = i;
            }
        }

        for (int f = 0; f < frame_count; f++)
            s->frame_state[f] = frames[f];
        s->frame_count = frame_count;
    }

    result.hit_ratio = (ref_len > 0)
        ? (double)(ref_len - result.page_faults) / ref_len * 100.0
        : 0.0;
    return result;
}

/* ─── Optimal ───────────────────────────────────────────────── */

/* Returns index into ref_str of next use of 'page' after position 'from',
   or INT_MAX if never used again. */
static int next_use(const int *ref_str, int ref_len, int page, int from) {
    for (int i = from + 1; i < ref_len; i++)
        if (ref_str[i] == page) return i;
    return 0x7FFFFFFF; /* "infinity" */
}

SimResult simulate_optimal(const int *ref_str, int ref_len, int frame_count) {
    SimResult result;
    memset(&result, 0, sizeof(result));
    result.steps = (StepInfo *)malloc(ref_len * sizeof(StepInfo));
    result.total_refs = ref_len;

    int frames[MAX_FRAMES];
    init_frames(frames, frame_count);

    for (int i = 0; i < ref_len; i++) {
        int page = ref_str[i];
        StepInfo *s = &result.steps[i];
        s->page_ref = page;

        int hit = find_in_frames(frames, frame_count, page);
        if (hit != -1) {
            s->is_fault = 0;
            s->replaced_page = EMPTY;
        } else {
            s->is_fault = 1;
            result.page_faults++;

            int empty = has_empty_frame(frames, frame_count);
            if (empty != -1) {
                frames[empty] = page;
                s->replaced_page = EMPTY;
            } else {
                /* Find frame whose page is used farthest in future */
                int farthest = -1, victim_idx = 0;
                for (int f = 0; f < frame_count; f++) {
                    int nu = next_use(ref_str, ref_len, frames[f], i);
                    if (nu > farthest) {
                        farthest = nu;
                        victim_idx = f;
                    }
                }
                s->replaced_page = frames[victim_idx];
                frames[victim_idx] = page;
            }
        }

        for (int f = 0; f < frame_count; f++)
            s->frame_state[f] = frames[f];
        s->frame_count = frame_count;
    }

    result.hit_ratio = (ref_len > 0)
        ? (double)(ref_len - result.page_faults) / ref_len * 100.0
        : 0.0;
    return result;
}

/* ─── Free ──────────────────────────────────────────────────── */

void free_result(SimResult *r) {
    if (r && r->steps) {
        free(r->steps);
        r->steps = NULL;
    }
}

/* ─── Print helpers (for standalone C binary) ──────────────── */

void print_result(const SimResult *r, const char *algo_name) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║  Algorithm : %-23s║\n", algo_name);
    printf("╠══════════════════════════════════════╣\n");
    printf("║  Step │ Page │ Frames           │ F ║\n");
    printf("╠══════════════════════════════════════╣\n");

    for (int i = 0; i < r->total_refs; i++) {
        const StepInfo *s = &r->steps[i];
        printf("║  %4d │  %3d │ ", i + 1, s->page_ref);
        for (int f = 0; f < s->frame_count; f++) {
            if (s->frame_state[f] == EMPTY) printf("  - ");
            else printf("%3d ", s->frame_state[f]);
        }
        /* pad if frame_count < MAX_FRAMES */
        for (int f = s->frame_count; f < 4; f++) printf("    ");
        printf("│ %c ║\n", s->is_fault ? 'F' : ' ');
    }

    printf("╠══════════════════════════════════════╣\n");
    printf("║  Page Faults : %-21d║\n", r->page_faults);
    printf("║  Hit Ratio   : %-20.2f%%║\n", r->hit_ratio);
    printf("╚══════════════════════════════════════╝\n");
}

/* ─── Main (standalone test driver) ────────────────────────── */

int main(void) {
    int ref[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int ref_len = sizeof(ref) / sizeof(ref[0]);
    int frames  = 3;

    printf("Reference String: ");
    for (int i = 0; i < ref_len; i++) printf("%d ", ref[i]);
    printf("\nFrame Count: %d\n", frames);

    SimResult fifo = simulate_fifo(ref, ref_len, frames);
    print_result(&fifo, "FIFO");
    free_result(&fifo);

    SimResult lru = simulate_lru(ref, ref_len, frames);
    print_result(&lru, "LRU");
    free_result(&lru);

    SimResult opt = simulate_optimal(ref, ref_len, frames);
    print_result(&opt, "Optimal");
    free_result(&opt);

    return 0;
}
