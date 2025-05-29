#include <stdio.h>

/* Simplified stubs corresponding to the original roff2.s
 * implementation of basic requests.  They currently only print a
 * message indicating they were invoked. */

static void stub(const char *name)
{
    printf("[stub] %s\n", name);
}

void casead(void) { stub("ad"); }
void casebr(void) { stub("br"); }
void casecc(void) { stub("cc"); }
void casece(void) { stub("ce"); }
void caseds(void) { stub("ds"); }
void casefi(void) { stub("fi"); }
void casein(void) { stub("in"); }
void caseli(void) { stub("li"); }
void casell(void) { stub("ll"); }
void casetr(void) { stub("tr"); }
/* many more would follow */
