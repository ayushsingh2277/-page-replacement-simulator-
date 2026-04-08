// #include <stdio.h>
// #include <string.h>
// #include "algorithms.h"

// static int in_frames(int *frames, int count, int page) {
//     for (int i = 0; i < count; i++)
//         if (frames[i] == page) return 1;
//     return 0;
// }

// Result run_fifo(int *ref, int n, int frame_count) {
//     Result result;
//     memset(&result, 0, sizeof(result));
//     strcpy(result.algorithm, "FIFO");

//     int frames[MAX_FRAMES];
//     int queue[MAX_FRAMES];
//     int filled = 0, q_size = 0;

//     for (int i = 0; i < frame_count; i++) frames[i] = -1;

//     for (int i = 0; i < n; i++) {
//         int page = ref[i];
//         Step *s = &result.steps[i];

//         s->step = i + 1;
//         s->page = page;

//         if (in_frames(frames, filled, page)) {
//             s->is_fault = 0;
//             s->replaced = -1;
//             result.hits++;
//         } else {
//             s->is_fault = 1;
//             result.faults++;

//             if (filled < frame_count) {
//                 frames[filled] = page;
//                 queue[q_size++] = page;
//                 filled++;
//                 s->replaced = -1;
//             } else {
//                 int oldest = queue[0];
//                 s->replaced = oldest;

//                 for (int j = 0; j < frame_count; j++) {
//                     if (frames[j] == oldest) {
//                         frames[j] = page;
//                         break;
//                     }
//                 }

//                 for (int j = 0; j < q_size - 1; j++)
//                     queue[j] = queue[j + 1];

//                 queue[q_size - 1] = page;
//             }
//         }

//         for (int j = 0; j < frame_count; j++)
//             s->frames[j] = frames[j];

//         s->frame_count = frame_count;
//     }

//     result.total_steps = n;
//     result.hit_rate = (result.hits * 100.0f) / n;
//     return result;
// }



#include <stdio.h> 
#include <string.h> 
#include "algorithms.h" 


/* ============================================================ 
* fifo.c  —  First In First Out Page Replacement 
 * ============================================================ 
 * 
 * HOW FIFO WORKS: 
 * - Maintain a queue: pages enter at the BACK, leave from FRONT 
 * - When a fault occurs and frames are full: 
 *     -> Remove the page at queue[0] (oldest arrival) 
 *     -> Shift all queue entries left 
 *     -> Add new page at the back 
 * 
 * DATA STRUCTURE: 
 *   frames[] = current pages in memory 
 *   queue[]  = tracks arrival order (queue[0] is always oldest) 
 * ============================================================ */ 
 
/* Helper: check if page is in frames array */ 
static int in_frames(int *frames, int count, int page) { 
    for (int i = 0; i < count; i++) 
        if (frames[i] == page) return 1; 
    return 0; 
} 
 
Result run_fifo(int *ref, int n, int frame_count) { 
    Result result; 
    memset(&result, 0, sizeof(result)); 
    strcpy(result.algorithm, "FIFO"); 
 
    int frames[MAX_FRAMES]; 
    int queue[MAX_FRAMES];     /* queue[0] = oldest page */ 
    int filled = 0;            /* how many frames are used */ 
    int q_size = 0;            /* current queue length */ 
 
    /* Initialize frames to -1 (empty) */ 
    for (int i = 0; i < frame_count; i++) 
        frames[i] = -1; 
 
    for (int i = 0; i < n; i++) { 
        int page = ref[i]; 
        Step *s  = &result.steps[i]; 
        s->step  = i + 1; 
        s->page  = page; 
 
        if (in_frames(frames, filled, page)) { 
            /* ── PAGE HIT ── */ 
            s->is_fault = 0; 
            s->replaced = -1; 
            result.hits++; 
 
        } else { 
            /* ── PAGE FAULT ── */ 
            s->is_fault = 1; 
            result.faults++; 
 
            if (filled < frame_count) { 
                /* Frames not full — just add the page */ 
                frames[filled] = page; 
                queue[q_size]  = page; 
                filled++; 
                q_size++; 
                s->replaced = -1; 
 
            } else { 
                /* Frames full — evict the oldest (front of queue) */ 
                int oldest = queue[0]; 
                s->replaced = oldest; 
                 
                /* Find and replace it in frames[] */ 
                for (int j = 0; j < frame_count; j++) { 
                    if (frames[j] == oldest) { 
                        frames[j] = page; 
                        break; 
                    } 
                } 
 
                /* Shift queue left and add new page at back */ 
                for (int j = 0; j < q_size - 1; j++) 
                    queue[j] = queue[j + 1]; 
                queue[q_size - 1] = page; 
            } 
        } 
 
        /* Save current frame state in this step */ 
        for (int j = 0; j < frame_count; j++) 
            s->frames[j] = frames[j]; 
        s->frame_count = frame_count; 
    } 
 
    result.total_steps = n; 
    result.hit_rate = (result.hits * 100.0f) / n; 
    return result; 
} 