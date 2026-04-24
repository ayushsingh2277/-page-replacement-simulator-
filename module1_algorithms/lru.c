

#include <stdio.h> 
#include <string.h> 
#include "algorithms.h" 
 
/* ============================================================ 
 * lru.c  —  Least Recently Used Page Replacement 
 * ============================================================ 
 * 
 * HOW LRU WORKS: 
 * - Track LAST TIME each frame's page was used (last_used[]) 
 * - When fault + frames full: 
 *     -> Find frame with SMALLEST last_used value 
 *     -> Replace that page with the new one 
 *     -> Update last_used for the new page 
 * 
 * DATA STRUCTURE: 
 *   frames[]    = current pages in memory 
 *   last_used[] = timestamp (step index) when each frame was 
 *                 last accessed. Smallest = least recent. 
 * ============================================================ */ 
 
static int in_frames_idx(int *frames, int count, int page) { 
    for (int i = 0; i < count; i++) 
        if (frames[i] == page) return i;   /* return index if found */ 
    return -1;                              /* -1 means not found    */ 
} 
 
Result run_lru(int *ref, int n, int frame_count) { 
    Result result; 
    memset(&result, 0, sizeof(result)); 
    strcpy(result.algorithm, "LRU"); 
 
    int frames[MAX_FRAMES]; 
     int last_used[MAX_FRAMES];  /* last_used[i] = step when frames[i] last used */ 
    int filled = 0; 
 
    for (int i = 0; i < frame_count; i++) { 
        frames[i]    = -1; 
        last_used[i] = -1; 
    } 
 
    for (int i = 0; i < n; i++) { 
        int page = ref[i]; 
        Step *s  = &result.steps[i]; 
        s->step  = i + 1; 
        s->page  = page; 
 
        int idx = in_frames_idx(frames, filled, page); 
 
        if (idx != -1) { 
            /* ── PAGE HIT — update last used time ── */ 
            s->is_fault   = 0; 
            s->replaced   = -1; 
            last_used[idx] = i; 
            result.hits++; 
 
        } else { 
            /* ── PAGE FAULT ── */ 
            s->is_fault = 1; 
            result.faults++; 
 
            if (filled < frame_count) { 
                /* Frames not full yet */ 
                frames[filled]    = page; 
                last_used[filled] = i; 
                filled++; 
                s->replaced = -1; 
 
            } else { 
                /* Find frame with MINIMUM last_used (least recently used) */ 
                int lru_idx  = 0; 
                int min_time = last_used[0]; 
 
                for (int j = 1; j < frame_count; j++) { 
                    if (last_used[j] < min_time) { 
                        min_time = last_used[j]; 
                        lru_idx  = j; 
                    } 
                } 
 
                s->replaced      = frames[lru_idx]; 
                frames[lru_idx]  = page; 
                last_used[lru_idx] = i; 
            } 
        } 
 
        /* Save frame snapshot */ 
        for (int j = 0; j < frame_count; j++) 
            s->frames[j] = frames[j]; 
        s->frame_count = frame_count; 
    } 
 
    result.total_steps = n; 
    result.hit_rate = (result.hits * 100.0f) / n; 
    return result; 
} 