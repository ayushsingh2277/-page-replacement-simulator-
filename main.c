/*
 * =============================================================================
 * Page Replacement Algorithm Simulator - Backend (Single File)
 * =============================================================================
 * This is a standalone single-file C implementation for the GUI backend.
 * It includes FIFO, LRU, and Optimal page replacement algorithms.
 *
 * The original modular project (module1_algorithms/, module2_display/,
 * module3_ui/) remains UNTOUCHED. This file is a NEW addition for GUI support.
 *
 * Usage:
 *   ./simulator <algorithm> <frames> <page1> <page2> ... <pageN>
 *
 *   algorithm: fifo | lru | optimal | all
 *   frames:    number of frames (e.g., 3)
 *   pages:     space-separated reference string (e.g., 1 2 3 4 1 2 5)
 *
 * Output format (structured for frontend parsing):
 *   Each step is printed as: STEP|page|hit_or_fault|frame0,frame1,...,frameN
 *   Summary line: SUMMARY|hits|faults|total|hit_rate
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── Constants ───────────────────────────────────────────────────────────── */
#define MAX_PAGES   100
#define MAX_FRAMES  10

/* ─── Helper: check if page is in frames ─────────────────────────────────── */
int is_in_frames(int *frames, int n_frames, int page) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i] == page) return 1;
    }
    return 0;
}

/* ─── Helper: print current frame state (comma-separated) ────────────────── */
void print_frames(int *frames, int n_frames) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i] == -1)
            printf("-");
        else
            printf("%d", frames[i]);
        if (i < n_frames - 1) printf(",");
    }
}

/* ─── FIFO Algorithm ──────────────────────────────────────────────────────── */
void run_fifo(int *pages, int n_pages, int n_frames) {
    int frames[MAX_FRAMES];
    int hits = 0, faults = 0;
    int next_replace = 0; /* FIFO pointer - points to oldest frame */

    /* Initialize all frames to -1 (empty) */
    for (int i = 0; i < n_frames; i++) frames[i] = -1;

    printf("ALGO|FIFO\n");

    for (int i = 0; i < n_pages; i++) {
        int page = pages[i];

        if (is_in_frames(frames, n_frames, page)) {
            /* HIT: page already in memory */
            hits++;
            printf("STEP|%d|HIT|", page);
        } else {
            /* FAULT: page not in memory, replace using FIFO */
            faults++;
            frames[next_replace] = page;
            next_replace = (next_replace + 1) % n_frames; /* Move pointer */
            printf("STEP|%d|FAULT|", page);
        }

        print_frames(frames, n_frames);
        printf("\n");
    }

    /* Print summary */
    double hit_rate = (n_pages > 0) ? (100.0 * hits / n_pages) : 0.0;
    printf("SUMMARY|%d|%d|%d|%.2f\n", hits, faults, n_pages, hit_rate);
}

/* ─── LRU Algorithm ───────────────────────────────────────────────────────── */
void run_lru(int *pages, int n_pages, int n_frames) {
    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES]; /* Tracks when each frame was last used */
    int hits = 0, faults = 0;

    /* Initialize all frames to -1 (empty) */
    for (int i = 0; i < n_frames; i++) {
        frames[i] = -1;
        last_used[i] = -1;
    }

    printf("ALGO|LRU\n");

    for (int i = 0; i < n_pages; i++) {
        int page = pages[i];
        int found = 0;

        /* Check if page is already in a frame */
        for (int j = 0; j < n_frames; j++) {
            if (frames[j] == page) {
                found = 1;
                last_used[j] = i; /* Update last used time */
                hits++;
                printf("STEP|%d|HIT|", page);
                break;
            }
        }

        if (!found) {
            /* FAULT: find LRU frame to replace */
            faults++;
            int replace_idx = 0;

            /* First, try to find an empty frame */
            int empty_found = 0;
            for (int j = 0; j < n_frames; j++) {
                if (frames[j] == -1) {
                    replace_idx = j;
                    empty_found = 1;
                    break;
                }
            }

            /* If no empty frame, find Least Recently Used */
            if (!empty_found) {
                int lru_time = last_used[0];
                for (int j = 1; j < n_frames; j++) {
                    if (last_used[j] < lru_time) {
                        lru_time = last_used[j];
                        replace_idx = j;
                    }
                }
            }

            frames[replace_idx] = page;
            last_used[replace_idx] = i;
            printf("STEP|%d|FAULT|", page);
        }

        print_frames(frames, n_frames);
        printf("\n");
    }

    double hit_rate = (n_pages > 0) ? (100.0 * hits / n_pages) : 0.0;
    printf("SUMMARY|%d|%d|%d|%.2f\n", hits, faults, n_pages, hit_rate);
}

/* ─── Optimal Algorithm ───────────────────────────────────────────────────── */
void run_optimal(int *pages, int n_pages, int n_frames) {
    int frames[MAX_FRAMES];
    int hits = 0, faults = 0;

    /* Initialize all frames to -1 (empty) */
    for (int i = 0; i < n_frames; i++) frames[i] = -1;

    printf("ALGO|OPTIMAL\n");

    for (int i = 0; i < n_pages; i++) {
        int page = pages[i];

        if (is_in_frames(frames, n_frames, page)) {
            /* HIT */
            hits++;
            printf("STEP|%d|HIT|", page);
        } else {
            /* FAULT: find optimal page to replace */
            faults++;
            int replace_idx = -1;

            /* First, try to find an empty frame */
            for (int j = 0; j < n_frames; j++) {
                if (frames[j] == -1) {
                    replace_idx = j;
                    break;
                }
            }

            if (replace_idx == -1) {
                /* Find the page used farthest in the future */
                int farthest = -1;

                for (int j = 0; j < n_frames; j++) {
                    int next_use = n_pages; /* Default: never used again */

                    /* Look ahead to find next use of frames[j] */
                    for (int k = i + 1; k < n_pages; k++) {
                        if (pages[k] == frames[j]) {
                            next_use = k;
                            break;
                        }
                    }

                    /* Pick the frame with the farthest (or no) future use */
                    if (next_use > farthest) {
                        farthest = next_use;
                        replace_idx = j;
                    }
                }
            }

            frames[replace_idx] = page;
            printf("STEP|%d|FAULT|", page);
        }

        print_frames(frames, n_frames);
        printf("\n");
    }

    double hit_rate = (n_pages > 0) ? (100.0 * hits / n_pages) : 0.0;
    printf("SUMMARY|%d|%d|%d|%.2f\n", hits, faults, n_pages, hit_rate);
}

/* ─── Main Entry Point ────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    /* Validate minimum arguments: ./simulator <algo> <frames> <page1...> */
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fifo|lru|optimal|all> <frames> <page1> [page2 ...]\n", argv[0]);
        fprintf(stderr, "Example: %s fifo 3 1 2 3 4 1 2 5\n", argv[0]);
        return 1;
    }

    char *algorithm = argv[1];
    int n_frames = atoi(argv[2]);

    /* Validate frame count */
    if (n_frames <= 0 || n_frames > MAX_FRAMES) {
        fprintf(stderr, "ERROR: Frames must be between 1 and %d\n", MAX_FRAMES);
        return 1;
    }

    /* Parse page reference string from remaining arguments */
    int pages[MAX_PAGES];
    int n_pages = 0;

    for (int i = 3; i < argc && n_pages < MAX_PAGES; i++) {
        pages[n_pages] = atoi(argv[i]);
        n_pages++;
    }

    if (n_pages == 0) {
        fprintf(stderr, "ERROR: No pages provided\n");
        return 1;
    }

    /* Print header info for frontend */
    printf("FRAMES|%d\n", n_frames);
    printf("PAGES|%d\n", n_pages);

    /* Run selected algorithm(s) */
    if (strcmp(algorithm, "fifo") == 0) {
        run_fifo(pages, n_pages, n_frames);

    } else if (strcmp(algorithm, "lru") == 0) {
        run_lru(pages, n_pages, n_frames);

    } else if (strcmp(algorithm, "optimal") == 0) {
        run_optimal(pages, n_pages, n_frames);

    } else if (strcmp(algorithm, "all") == 0) {
        /* Compare All: run all three algorithms, separated by a divider */
        run_fifo(pages, n_pages, n_frames);
        printf("---\n"); /* Divider between algorithms */
        run_lru(pages, n_pages, n_frames);
        printf("---\n");
        run_optimal(pages, n_pages, n_frames);

    } else {
        fprintf(stderr, "ERROR: Unknown algorithm '%s'. Use fifo, lru, optimal, or all\n", algorithm);
        return 1;
    }

    return 0;
}
