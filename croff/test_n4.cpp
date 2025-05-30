#include "cxx23_scaffold.hpp"
/*
 * test_n4.c - Unit tests for n4.c number register and arithmetic functions
 *
 * Tests for the troff number register system, arithmetic evaluation,
 * and number formatting functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Mock the required headers and definitions */
#ifndef NC
#define NC 512
#endif
#ifndef NN
#define NN 200
#endif
#ifndef NNAMES
#define NNAMES 10
#endif
#ifndef CMASK
#define CMASK 0377
#endif
#ifndef BMASK
#define BMASK 0377
#endif
#ifndef BYTE
#define BYTE 8
#endif
#ifndef EM
#define EM 200
#endif
#ifndef VERT
#define VERT 144
#endif
#ifndef HOR
#define HOR 240
#endif
#ifndef INCH
#define INCH 240
#endif

/* Mock global variables */
int ascii = 0;
extern int cbuf[NC];
extern int *cp;
int r[NN];
int vlist[NN];
int inc[NN];
int fmt[NN];
int ch = 0;
int lgf = 0;
int frame_data = 0;
int *frame = &frame_data;
int pl = 66;
int lastl = 80;
int ralss = 0;
int totout = 0;
int nrbits = 0;
int nonumb = 0;
int vflag = 0;
int noscale = 0;
int dfact = 1;
int dfactd = 1;
/* Mock environment structure */
#ifndef ENV_DEFINED
#define ENV_DEFINED
struct env {
    int hnl;
    int dnl;
    int op;
    int curd;
} env_data = {0, 0, 0, 0};
struct env *dip = &env_data;
#endif
int fi = 1;
int res = 1;
int cwidth = 10;
int dotT = 37;
int ulfont = 2;
int ev = 0;
int ne = 0;
int ad = 1, admod = 0;
int print = 1;
int ls = 1;
int xxx = 0;

/* Mock v structure */
struct {
    int cd;
    int nl;
    int hp;
} v = {1, 100, 50};

/* Test input buffer */
static char test_input[256];
static int test_pos = 0;

/* Mock functions */
int getch(void) {
    if (ch) {
        int ret = ch;
        ch = 0;
        return ret;
    }
    if (test_pos < strlen(test_input)) {
        return test_input[test_pos++];
    }
    return 0;
}

int getsn(void) {
    return getch() | (getch() << 8);
}

int getrq(void) {
    return getch() | (getch() << 8);
}

int findt1(void) { return 100; }
int skip(void) { return 0; }
int alph(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
void prstrfl(const char *s) { printf("%s", s); }
void done2(int code) { exit(code); }
void edone(int code) { /* recoverable error */ }

/* Include the functions under test */
#include "n4.c"

/* Test utilities */
void reset_test_state(void) {
    memset(cbuf, 0, sizeof(cbuf));
    cp = cbuf;
    memset(r, 0, sizeof(r));
    memset(vlist, 0, sizeof(vlist));
    ch = 0;
    nonumb = 0;
    nform = 0;
    test_pos = 0;
    memset(test_input, 0, sizeof(test_input));
}

void set_test_input(const char *input) {
    strcpy(test_input, input);
    test_pos = 0;
}

/* Test functions */

void test_wrc(void) {
    printf("Testing wrc()...\n");

    reset_test_state();

    /* Test normal character write */
    assert(wrc('A') == 1);
    assert(cbuf[0] == 'A');
    assert(cp == &cbuf[1]);

    /* Test multiple characters */
    assert(wrc('B') == 1);
    assert(wrc('C') == 1);
    assert(cbuf[1] == 'B');
    assert(cbuf[2] == 'C');

    printf("wrc() tests passed.\n");
}

void test_quant(void) {
    printf("Testing quant()...\n");

    /* Test positive numbers */
    assert(quant(15, 10) == 20); /* Round up */
    assert(quant(14, 10) == 10); /* Round down */
    assert(quant(15, 6) == 18); /* Round up */
    assert(quant(12, 6) == 12); /* Exact multiple */

    /* Test negative numbers */
    assert(quant(-15, 10) == -20); /* Round down (more negative) */
    assert(quant(-14, 10) == -10); /* Round up (less negative) */

    /* Test zero */
    assert(quant(0, 10) == 0);

    printf("quant() tests passed.\n");
}

void test_findr(void) {
    printf("Testing findr()...\n");

    reset_test_state();

    /* Test invalid register name */
    assert(findr(0) == -1);

    /* Test new register creation */
    int idx1 = findr(0x100); /* Register "aa" */
    assert(idx1 >= 0);
    assert(r[idx1] == 0x100);

    /* Test finding existing register */
    int idx2 = findr(0x100);
    assert(idx2 == idx1);

    /* Test different register */
    int idx3 = findr(0x200); /* Register "bb" */
    assert(idx3 >= 0);
    assert(idx3 != idx1);
    assert(r[idx3] == 0x200);

    printf("findr() tests passed.\n");
}

void test_decml(void) {
    printf("Testing decml()...\n");

    reset_test_state();
    nform = 0;

    /* Test single digit */
    int count = decml(5, wrc);
    assert(count == 1);
    assert(cbuf[0] == '5');

    /* Test multiple digits */
    reset_test_state();
    nform = 0;
    count = decml(123, wrc);
    assert(count == 3);
    assert(cbuf[0] == '1');
    assert(cbuf[1] == '2');
    assert(cbuf[2] == '3');

    /* Test zero */
    reset_test_state();
    nform = 0;
    count = decml(0, wrc);
    assert(count == 1);
    assert(cbuf[0] == '0');

    printf("decml() tests passed.\n");
}

void test_roman(void) {
    printf("Testing roman()...\n");

    reset_test_state();

    /* Test zero */
    nform = 'i';
    int count = roman(0, wrc);
    assert(count == 1);
    assert(cbuf[0] == '0');

    /* Test lowercase roman numerals */
    reset_test_state();
    nform = 'i';
    count = roman(1, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'i');

    reset_test_state();
    nform = 'i';
    count = roman(4, wrc);
    assert(count == 2);
    assert(cbuf[0] == 'i');
    assert(cbuf[1] == 'v');

    reset_test_state();
    nform = 'i';
    count = roman(9, wrc);
    assert(count == 2);
    assert(cbuf[0] == 'i');
    assert(cbuf[1] == 'x');

    /* Test uppercase roman numerals */
    reset_test_state();
    nform = 'I';
    count = roman(5, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'V');

    printf("roman() tests passed.\n");
}

void test_abc(void) {
    printf("Testing abc()...\n");

    reset_test_state();

    /* Test zero */
    nform = 'a';
    int count = abc(0, wrc);
    assert(count == 1);
    assert(cbuf[0] == '0');

    /* Test lowercase alphabetic */
    reset_test_state();
    nform = 'a';
    count = abc(1, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'a');

    reset_test_state();
    nform = 'a';
    count = abc(26, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'z');

    /* Test uppercase alphabetic */
    reset_test_state();
    nform = 'A';
    count = abc(1, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'A');

    printf("abc() tests passed.\n");
}

void test_fnumb(void) {
    printf("Testing fnumb()...\n");

    reset_test_state();

    /* Test positive decimal */
    nform = '1';
    int count = fnumb(123, wrc);
    assert(count == 3);
    assert(cbuf[0] == '1');
    assert(cbuf[1] == '2');
    assert(cbuf[2] == '3');

    /* Test negative decimal */
    reset_test_state();
    nform = '1';
    count = fnumb(-45, wrc);
    assert(count == 3);
    assert(cbuf[0] == '-');
    assert(cbuf[1] == '4');
    assert(cbuf[2] == '5');

    /* Test roman format */
    reset_test_state();
    nform = 'i';
    count = fnumb(5, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'v');

    /* Test alphabetic format */
    reset_test_state();
    nform = 'a';
    count = fnumb(3, wrc);
    assert(count == 1);
    assert(cbuf[0] == 'c');

    printf("fnumb() tests passed.\n");
}

void test_setn1(void) {
    printf("Testing setn1()...\n");

    reset_test_state();

    /* Test decimal conversion */
    nform = 0;
    setn1(456);
    assert(cbuf[0] == '4');
    assert(cbuf[1] == '5');
    assert(cbuf[2] == '6');
    assert(cbuf[3] == 0); /* Null terminated */
    assert(cp == cbuf); /* Reset to start */

    printf("setn1() tests passed.\n");
}

void test_atoi1_basic(void) {
    printf("Testing atoi1() basic parsing...\n");

    reset_test_state();
    dfact = dfactd = 1;
    noscale = 0;

    /* Test simple positive number */
    set_test_input("123");
    long result = atoi1();
    assert(result == 123);
    assert(nonumb == 0);

    /* Test negative number */
    reset_test_state();
    set_test_input("-456");
    result = atoi1();
    assert(result == -456);
    assert(nonumb == 0);

    /* Test no number */
    reset_test_state();
    set_test_input("xyz");
    result = atoi1();
    assert(result == 0);
    assert(nonumb == 1);

    printf("atoi1() basic tests passed.\n");
}

void test_atoi1_scaling(void) {
    printf("Testing atoi1() scaling units...\n");

    reset_test_state();
    dfact = dfactd = 1;
    noscale = 0;
    lss = 12;

    /* Test with 'u' unit */
    set_test_input("100u");
    long result = atoi1();
    assert(result == 100);

    /* Test with 'v' unit */
    reset_test_state();
    set_test_input("5v");
    result = atoi1();
    assert(result == 5 * lss);

    /* Test with 'm' unit */
    reset_test_state();
    set_test_input("2m");
    result = atoi1();
    assert(result == 2 * EM);

    /* Test with 'p' unit (points) */
    reset_test_state();
    set_test_input("72p");
    result = atoi1();
    assert(result == INCH);

    /* Test with 'i' unit (inches) */
    reset_test_state();
    set_test_input("1i");
    result = atoi1();
    assert(result == INCH);

    printf("atoi1() scaling tests passed.\n");
}

void test_atoi0_arithmetic(void) {
    printf("Testing atoi0() arithmetic...\n");

    /* Test simple addition */
    reset_test_state();
    set_test_input("10+5");
    long result = atoi0();
    assert(result == 15);

    /* Test subtraction */
    reset_test_state();
    set_test_input("20-8");
    result = atoi0();
    assert(result == 12);

    /* Test multiplication */
    reset_test_state();
    set_test_input("6*7");
    result = atoi0();
    assert(result == 42);

    /* Test division */
    reset_test_state();
    set_test_input("24/4");
    result = atoi0();
    assert(result == 6);

    /* Test modulo */
    reset_test_state();
    set_test_input("17%5");
    result = atoi0();
    assert(result == 2);

    printf("atoi0() arithmetic tests passed.\n");
}

void test_atoi0_comparison(void) {
    printf("Testing atoi0() comparison operators...\n");

    /* Test equality */
    reset_test_state();
    set_test_input("5=5");
    long result = atoi0();
    assert(result == 1);

    reset_test_state();
    set_test_input("5=3");
    result = atoi0();
    assert(result == 0);

    /* Test greater than */
    reset_test_state();
    set_test_input("7>3");
    result = atoi0();
    assert(result == 1);

    reset_test_state();
    set_test_input("3>7");
    result = atoi0();
    assert(result == 0);

    /* Test less than */
    reset_test_state();
    set_test_input("3<7");
    result = atoi0();
    assert(result == 1);

    reset_test_state();
    set_test_input("7<3");
    result = atoi0();
    assert(result == 0);

    printf("atoi0() comparison tests passed.\n");
}

void test_atoi0_logical(void) {
    printf("Testing atoi0() logical operators...\n");

    /* Test logical AND */
    reset_test_state();
    set_test_input("5&3");
    long result = atoi0();
    assert(result == 1); /* Both positive */

    reset_test_state();
    set_test_input("5&0");
    result = atoi0();
    assert(result == 0); /* One is zero */

    /* Test logical OR */
    reset_test_state();
    set_test_input("5:0");
    result = atoi0();
    assert(result == 1); /* One is positive */

    reset_test_state();
    set_test_input("0:0");
    result = atoi0();
    assert(result == 0); /* Both are zero */

    printf("atoi0() logical tests passed.\n");
}

void test_inumb(void) {
    printf("Testing inumb()...\n");

    reset_test_state();
    res = 1;
    dfact = dfactd = 1;

    /* Test absolute value */
    set_test_input("100");
    int result = inumb(NULL);
    assert(result == 100);

    /* Test relative increment */
    int base_val = 50;
    reset_test_state();
    set_test_input("+25");
    result = inumb(&base_val);
    assert(result == 75);

    /* Test relative decrement */
    base_val = 50;
    reset_test_state();
    set_test_input("-15");
    result = inumb(&base_val);
    assert(result == 35);

    printf("inumb() tests passed.\n");
}

int main(void) {
    printf("Starting n4.c unit tests...\n\n");

    test_wrc();
    test_quant();
    test_findr();
    test_decml();
    test_roman();
    test_abc();
    test_fnumb();
    test_setn1();
    test_atoi1_basic();
    test_atoi1_scaling();
    test_atoi0_arithmetic();
    test_atoi0_comparison();
    test_atoi0_logical();
    test_inumb();

    printf("\nAll tests passed successfully!\n");
    return 0;
}
