#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* prototypes for helper routines provided in runtime.c */
void mesg(int);
int dsp(int);
void flush_output(char *buf, size_t *p);

/*
 * Simplified translation of the original roff1.s module.  This only
 * provides a minimal demonstration of the logic: initialise the runtime
 * and copy standard input to standard output using a small buffer.
 */
int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    char buf[256];
    size_t pos = 0;

    /* mimic the original start-up by disabling messages on the tty */
    mesg(0);

    int c;
    while ((c = getchar()) != EOF) {
        buf[pos++] = (char)c;
        if (c == '\n' || pos >= sizeof(buf))
            flush_output(buf, &pos);
    }
    flush_output(buf, &pos);

    /* restore terminal write permissions */
    mesg(1);
    return 0;
}
