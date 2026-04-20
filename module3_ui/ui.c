<<<<<<< HEAD
=======
```c
>>>>>>> module3-ui
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "../module1_algorithms/algorithms.h"
#include "../module2_display/display.h"

<<<<<<< HEAD
#define MAX_PAGES 100
#define MAX_FRAMES 10

int parse_reference_string(char *input, int *ref_array) {
    int count = 0;
    char *token = strtok(input, " ");
=======
/* ============================================================
 * ui.c  —  Module 3: UI & Controller
 * ============================================================ */

/* ✅ Correct ANSI color codes */
#define CYAN   "\033[0;36m"
#define BOLD   "\033[1m"
#define YELLOW "\033[1;33m"
#define RESET  "\033[0m"

/* ── PARSE REFERENCE STRING ── */
int parse_reference_string(char *input, int *ref_array) {
    int count = 0;
    char *token = strtok(input, " ");

>>>>>>> module3-ui
    while (token != NULL && count < MAX_PAGES) {
        ref_array[count++] = atoi(token);
        token = strtok(NULL, " ");
    }
    return count;
}

<<<<<<< HEAD
void show_about(void) {
    printf("\nPage Replacement Simulator\n");
    printf("Algorithms: FIFO, LRU, Optimal\n");
}

static void print_menu(void) {
    printf("\n1. FIFO\n");
    printf("2. LRU\n");
    printf("3. Optimal\n");
    printf("4. Compare All\n");
    printf("5. About\n");
    printf("6. Exit\n");
    printf("Enter choice: ");
}

static int get_inputs(int *ref, int *n, int *frames) {
    char input[500];

    printf("Enter reference string: ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = 0;

    *n = parse_reference_string(input, ref);

    printf("Enter frames: ");
    scanf("%d", frames);
    getchar();

    return (*n > 0 && *frames > 0);
}

void run_menu(void) {
    int choice;
    int ref[MAX_PAGES], n, frames;

    while (1) {
        print_menu();
        scanf("%d", &choice);
        getchar();

        if (choice == 6) break;
=======
/* ── MAIN MENU ── */
static void print_menu(void) {
    printf("\n%s+========================================+%s\n", CYAN, RESET);
    printf("%s|  PAGE REPLACEMENT ALGORITHM SIMULATOR  |%s\n", CYAN, RESET);
    printf("%s|         CA2 Project — BTech CSE         |%s\n", CYAN, RESET);
    printf("%s+========================================+%s\n", CYAN, RESET);

    printf("  1. Run FIFO Algorithm\n");
    printf("  2. Run LRU Algorithm\n");
    printf("  3. Run Optimal Algorithm\n");
    printf("  4. Compare All Three Algorithms\n");
    printf("  5. About / Help\n");
    printf("  6. Exit\n");

    printf("%s+========================================+%s\n", CYAN, RESET);
    printf("  Enter your choice (1-6): ");
}

/* ── GET INPUT FROM USER ── */
static int get_inputs(int *ref, int *n, int *frames) {
    char input_buf[512];

    printf("\n  Enter reference string (numbers separated by spaces):\n  > ");
    fgets(input_buf, sizeof(input_buf), stdin);

    /* Remove newline */
    input_buf[strcspn(input_buf, "\n")] = 0;

    *n = parse_reference_string(input_buf, ref);

    if (*n == 0) {
        printf("  [ERROR] No valid pages entered.\n");
        return 0;
    }

    printf("  Enter number of frames (1-10): ");

    if (scanf("%d", frames) != 1) {
        printf("  [ERROR] Invalid input.\n");
        while (getchar() != '\n'); // clear buffer
        return 0;
    }

    while (getchar() != '\n'); // clear buffer

    if (*frames < 1 || *frames > MAX_FRAMES) {
        printf("  [ERROR] Frames must be between 1 and 10.\n");
        return 0;
    }

    return 1;
}

/* ── ABOUT SCREEN ── */
void show_about(void) {
    printf("\n%s+====== ABOUT ======+%s\n", CYAN, RESET);
    printf("  %sPage Replacement Algorithm Simulator%s\n", BOLD, RESET);
    printf("  Subject  : Operating Systems\n");
    printf("  Language : C (C99)\n");
    printf("  Platform : Linux / WSL2\n");

    printf("\n  %sAlgorithms:%s\n", BOLD, RESET);
    printf("  FIFO    - First In First Out\n");
    printf("  LRU     - Least Recently Used\n");
    printf("  Optimal - Belady's Algorithm\n");

    printf("\n  %sTeam:%s\n", BOLD, RESET);
    printf("  Member 1 - Algorithm Engine  (fifo.c, lru.c, optimal.c)\n");
    printf("  Member 2 - Display & Reports (display.c)\n");
    printf("  Member 3 - UI Controller     (ui.c, main.c)\n");

    printf("%s+===================+%s\n\n", CYAN, RESET);
}

/* ── RUN MENU LOOP ── */
void run_menu(void) {
    int choice;
    int ref[MAX_PAGES];
    int n, frames;

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            printf("  [ERROR] Invalid input. Enter 1-6.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        while (getchar() != '\n'); // clear buffer

        if (choice == 6) {
            printf("\n  Exiting. Goodbye!\n\n");
            break;
        }

>>>>>>> module3-ui
        if (choice == 5) {
            show_about();
            continue;
        }

<<<<<<< HEAD
=======
        if (choice < 1 || choice > 4) {
            printf("  [ERROR] Enter a number between 1 and 6.\n");
            continue;
        }

        /* Get inputs */
>>>>>>> module3-ui
        if (!get_inputs(ref, &n, &frames)) continue;

        Result r1, r2, r3;

        switch (choice) {
            case 1:
                r1 = run_fifo(ref, n, frames);
                display_result(&r1);
                break;

            case 2:
                r2 = run_lru(ref, n, frames);
                display_result(&r2);
                break;

            case 3:
                r3 = run_optimal(ref, n, frames);
                display_result(&r3);
                break;

            case 4:
                r1 = run_fifo(ref, n, frames);
                r2 = run_lru(ref, n, frames);
                r3 = run_optimal(ref, n, frames);

                display_result(&r1);
                display_result(&r2);
                display_result(&r3);
                display_comparison(&r1, &r2, &r3);
                break;
        }
    }
<<<<<<< HEAD
}
=======
}
```
>>>>>>> module3-ui
