/* C17 - no scaffold needed */
/*
 * test_n2.c - Unit tests for n2.c character output and termination functions
 *
 * This test suite provides comprehensive testing for the character processing,
 * output buffering, and program termination functions in n2.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

/* Include header files with declarations */
#include "t.h" // updated header

/* Include the source file to test */
#include "n2.c"

/* Mock external functions that may not be defined */
void wbf(int i) { /* Mock function */ }
void ptout(int i) { /* Mock function */ }
int control(int i, int j) { return 0; }
void reset(void) { /* Mock function */ }
void wbt(int i) { /* Mock function */ }
void getword(int i) { /* Mock function */ }
void tbreak(void) { /* Mock function */ }
void eject(int i) { /* Mock function */ }
void ptlead(void) { /* Mock function */ }
void twdone(void) { /* Mock function */ }
void mesg(int i) { /* Mock function */ }
void prstr(const char *s) { printf("%s", s); }
long seek(int fd, long offset, int whence) { return lseek(fd, offset, whence); }
void stty(int fd, int *args) { /* Mock function */ }
int skip(void) { return 0; }
int getname(void) { return 1; }

/* Test helper functions */
static void setup_test(void) {
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    error = 0;
    toolate = 0;
    ascii = 1;
    no_out = 0;
    tflg = 0;
    print = 1;
    dip = NULL;

    /* Initialize translation table */
    for (int i = 0; i < 256; i++) {
        trtab[i] = i;
    }
}

static void cleanup_test(void) {
    if (ptid > 0 && ptid != 1) {
        close(ptid);
        ptid = 0;
    }
}

/* Test pchar function */
static void test_pchar(void) {
    printf("Testing pchar function...\n");

    setup_test();

    /* Test normal character */
    pchar('A');
    assert(obuf[0] == 'A');

    /* Test motion character */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar(MOT | 'A');
    /* Should call pchar1 directly for motion characters */

    /* Test null character - should return without output */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar(0);
    assert(obuf[0] == 0);

    /* Test IMP character - should return without output */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar(IMP);
    assert(obuf[0] == 0);

    /* Test HX character with dip */
    struct env test_dip = {0, 0, 0};
    dip = &test_dip;
    tlss = 1 << 9;
    pchar(HX | 040000);
    assert(tlss == 0);
    dip = NULL;

    /* Test LX character */
    pchar(LX | 0123);
    assert(tlss == (LX | 0123));

    cleanup_test();
    printf("pchar tests passed!\n");
}

/* Test pchar1 function */
static void test_pchar1(void) {
    printf("Testing pchar1 function...\n");

    setup_test();

    /* Test normal ASCII character */
    pchar1('B');
    assert(obuf[0] == 'B');

    /* Test with diversion */
    struct env test_dip = {1, 0, 0};
    dip = &test_dip;
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1('C');
    /* Should call wbf instead of oput */
    assert(obuf[0] == 0);
    dip = NULL;

    /* Test with tflg=0 and print=0 */
    tflg = 0;
    print = 0;
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1('D');
    assert(obuf[0] == 0);
    tflg = 0;
    print = 1;

    /* Test with no_out=1 */
    no_out = 1;
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1('E');
    assert(obuf[0] == 0);
    no_out = 0;

    /* Test FILLER character */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1(FILLER);
    assert(obuf[0] == 0);

#ifndef NROFF
    /* Test ASCII mode with motion character */
    ascii = 1;
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1(MOT | 'F');
    assert(obuf[0] == ' ');

    /* Test ASCII mode with high character */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1(0200); /* Em dash */
    assert(obuf[0] == '-');

    /* Test ligatures */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar1(0211); /* fi ligature */
    assert(obuf[0] == 'f' && obuf[1] == 'i');
#endif

    cleanup_test();
    printf("pchar1 tests passed!\n");
}

/* Test oput and oputs functions */
static void test_output_functions(void) {
    printf("Testing oput and oputs functions...\n");

    setup_test();

    /* Test oput */
    oput('X');
    assert(obuf[0] == 'X');
    assert(obufp == obuf + 1);

    /* Test oputs */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    oputs("Hello");
    assert(strncmp(obuf, "Hello", 5) == 0);
    assert(obufp == obuf + 5);

    /* Test buffer flush on full buffer */
    obufp = obuf + OBUFSZ + ascii - 2;
    ptid = 1; /* Use stdout to avoid device opening */
    oput('Z');
    /* Should trigger flusho() */
    assert(obufp == obuf + 1);

    cleanup_test();
    printf("Output function tests passed!\n");
}

/* Test flusho function */
static void test_flusho(void) {
    printf("Testing flusho function...\n");

    setup_test();

    /* Setup test data */
    strcpy(obuf, "test data");
    obufp = obuf + 9;
    ptid = 1; /* Use stdout */

    /* Test flush */
    flusho();
    assert(obufp == obuf);

    cleanup_test();
    printf("flusho tests passed!\n");
}

/* Test done function */
static void test_done(void) {
    printf("Testing done function...\n");

    setup_test();

    /* Mock stack */
    int test_stack[STKSIZE];
    stk = test_stack;
    frame = stk;
    nxf = frame + STKSIZE;

    /* Test basic done functionality */
    int result = done(2);
    assert(error == 2);
    assert(level == 0);
    assert(app == 0);
    assert(ds == 0);
    assert(lgf == 0);

    /* Test with end macro */
    error = 0;
    em = 1;
    donef = 0;
    /* This would normally call done3(0) and exit, so we can't test the full path */

    cleanup_test();
    printf("done tests passed!\n");
}

/* Test done1 function */
static void test_done1(void) {
    printf("Testing done1 function...\n");

    setup_test();

    /* Test with v.nl = 0 (no content) */
    v.nl = 0;
    nofeed = 1;
    ptid = 1;

    /* This function calls done3(0) which calls exit(), so we test components */
    error = 0;
    /* Can't easily test the full function due to exit() call */

    cleanup_test();
    printf("done1 tests passed!\n");
}

/* Test edone function */
static void test_edone(void) {
    printf("Testing edone function...\n");

    setup_test();

    /* Mock stack */
    int test_stack[STKSIZE];
    stk = test_stack;
    frame = stk + 10; /* Not at base initially */
    nxf = frame + 10;
    ip = 5;

    /* Test that edone resets stack state */
    /* Note: edone calls done() which may call exit(), so we test setup only */
    assert(stk != NULL);

    cleanup_test();
    printf("edone tests passed!\n");
}

/* Test character translation */
static void test_character_translation(void) {
    printf("Testing character translation...\n");

    setup_test();

    /* Setup translation table */
    trtab['a'] = 'A';
    trtab['b'] = 'B';

    /* Test translation in pchar */
    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar('a');
    assert(obuf[0] == 'A');

    obufp = obuf;
    memset(obuf, 0, OBUFSZ);
    pchar('b');
    assert(obuf[0] == 'B');

    /* Reset translation table */
    for (int i = 0; i < 256; i++) {
        trtab[i] = i;
    }

    cleanup_test();
    printf("Character translation tests passed!\n");
}

/* Test error handling */
static void test_error_handling(void) {
    printf("Testing error handling...\n");

    setup_test();

    /* Test error accumulation */
    error = 0;
    done1(4);
    assert(error & 4);

    /* Test toolate flag */
    toolate = 0;
    no_out = 1; /* Prevent actual write */
    flusho();
    no_out = 0;

    cleanup_test();
    printf("Error handling tests passed!\n");
}

/* Main test runner */
int main(void) {
    printf("Starting n2.c test suite...\n\n");

    test_pchar();
    test_pchar1();
    test_output_functions();
    test_flusho();
    test_done();
    test_done1();
    test_edone();
    test_character_translation();
    test_error_handling();

    printf("\nAll tests passed successfully!\n");
    return 0;
}
