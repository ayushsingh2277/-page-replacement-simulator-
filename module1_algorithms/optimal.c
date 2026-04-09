


#include <stdio.h> 
#include <string.h> 
#include "algorithms.h" 
 
/* ============================================================ 
 * optimal.c  —  Optimal (Belady's) Page Replacement 
 * ============================================================ 
 * 
 * HOW OPTIMAL WORKS: 
 * - Look AHEAD in ref[] from current position 
 * - When fault + frames full: 
 *     -> For each frame, find the NEXT time that page appears 
 *     -> Replace the page with the FARTHEST (or never) next use 
 * 
 * DATA STRUCTURE: 
 *   frames[] = current pages in memory 
 *   next_use() = helper that scans forward in ref[] 
 * ============================================================ */ 
 
/* Return the next index in ref[] where 'page' appears after 'from' 
   Returns MAX_PAGES+1 (infinity) if page never appears again */ 
static int next_use(int *ref, int n, int from, int page) { 
    for (int k = from + 1; k < n; k++) 
        if (ref[k] == page) return k; 
    return MAX_PAGES + 1;   /* Will never be used again — evict first */ 
} 
 
static int in_frames_idx(int *frames, int count, int page) { 
    for (int i = 0; i < count; i++) 
        if (frames[i] == page) return i; 
    return -1; 
} 
 
Result run_optimal(int *ref, int n, int frame_count) { 
    Result result; 
    memset(&result, 0, sizeof(result)); 
    strcpy(result.algorithm, "Optimal"); 
 
    int frames[MAX_FRAMES]; 
    int filled = 0; 
 
    for (int i = 0; i < frame_count; i++) 
        frames[i] = -1; 
 
    for (int i = 0; i < n; i++) { 
        int page = ref[i]; 
        Step *s  = &result.steps[i]; 
        s->step  = i + 1; 
        s->page  = page; 
 
        int idx = in_frames_idx(frames, filled, page); 
 
        if (idx != -1) { 
            /* ── PAGE HIT ── */ 
            s->is_fault = 0; 
            s->replaced = -1; 
            result.hits++; 
 
        } else { 
            /* ── PAGE FAULT ── */ 
            s->is_fault = 1; 
                 result.faults++; 
 
            if (filled < frame_count) { 
                frames[filled] = page; 
                filled++; 
                s->replaced = -1; 
 
            } else { 
                /* Find frame whose page is used FARTHEST in future */ 
                int opt_idx   = 0; 
                int max_dist  = next_use(ref, n, i, frames[0]); 
 
                for (int j = 1; j < frame_count; j++) { 
                    int dist = next_use(ref, n, i, frames[j]); 
                    if (dist > max_dist) { 
                        max_dist = dist; 
                        opt_idx  = j; 
                    } 
                } 
 
                s->replaced    = frames[opt_idx]; 
                frames[opt_idx] = page; 
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