#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "../module1_algorithms/algorithms.h"
#include "../module2_display/display.h"

#define MAX_PAGES 100
#define MAX_FRAMES 10

int parse_reference_string(char *input, int *ref_array) {
    int count = 0;
    char *token = strtok(input, " ");
    while (token != NULL && count < MAX_PAGES) {
        ref_array[count++] = atoi(token);
        token = strtok(NULL, " ");
    }
    return count;
}

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
        if (choice == 5) {
            show_about();
            continue;
        }

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
}