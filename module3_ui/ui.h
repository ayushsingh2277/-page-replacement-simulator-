#ifndef UI_H 
#define UI_H 
 
/* ============================================================ 
 * ui.h  —  Module 3: UI & Controller 
 * Author : Member 3 
 * ============================================================ */ 
 
/* Parse a space-separated reference string from user input 
 * into an integer array. Returns number of pages parsed. */ 
int parse_reference_string(char *input, int *ref_array); 
 
/* Display and run the main menu */ 
void run_menu(void); 
 
/* Show the about/help screen */ 
void show_about(void); 
 
#endif /* UI_H */ 