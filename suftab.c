/*
 * suftab.c - Suffix table for text processing
 * Modernized to C90 standards
 */

#include <stdio.h>
#include <stdlib.h>
#include "suftab.h"

/* Suffix table data - modernized array declaration */
const char suftab[4096] = {
    // ...existing code...
};

/* Function to print suffix table information */
void print_suftab_info(void)
{
    printf("Suffix table size: %lu bytes\n", (unsigned long)sizeof(suftab));
    printf("First few bytes: %d, %d, %d, %d\n", (unsigned char)suftab[0], (unsigned char)suftab[1], (unsigned char)suftab[2], (unsigned char)suftab[3]);
}
