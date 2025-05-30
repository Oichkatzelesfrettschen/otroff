#include "cxx23_scaffold.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Mock definitions for testing
int yyval;
int ps = 10;
int dbg = 0;
int eht[100];
int ebase[100];
static int alloc_counter = 1;

// Mock functions
int oalloc() { return alloc_counter++; }
void ofree(int n) { /* mock free */ }
void nrwid(int a, int b, int c) { /* mock nrwid */ }
int max(int a, int b) { return (a > b) ? a : b; }
int VERT(int x) { return x * 2; }

// Function prototypes
void fromto(int p1, int p2, int p3);
void paren(int leftc, int p1, int rightc);
void brack(int m, char *t, char *c, char *b);
void diacrit(int p1, int type);
void move(int dir, int amt, char *p);
void funny(int n);

// Test helper to capture printf output
static char output_buffer[4096];
static int output_pos = 0;

int test_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(output_buffer + output_pos,
                           sizeof(output_buffer) - output_pos, format, args);
    output_pos += result;
    va_end(args);
    return result;
}

#define printf test_printf

void reset_output() {
    memset(output_buffer, 0, sizeof(output_buffer));
    output_pos = 0;
}

void test_fromto() {
    printf("Testing fromto function...\n");

    // Initialize test data
    eht[1] = 10;
    ebase[1] = 5;
    eht[2] = 8;
    ebase[2] = 3;
    eht[3] = 6;
    ebase[3] = 2;

    reset_output();
    fromto(1, 2, 3);

    assert(yyval > 0);
    assert(eht[yyval] == 10 + 8 + 6);
    assert(ebase[yyval] == 8 + 5);

    printf("fromto test passed\n");
}

void test_paren() {
    printf("Testing paren function...\n");

    // Test with parentheses
    eht[1] = 20;
    ebase[1] = 10;
    reset_output();
    paren('(', 1, ')');

    assert(yyval == 1);
    assert(eht[yyval] > 0);
    assert(ebase[yyval] >= 0);

    // Test with braces
    reset_output();
    paren('{', 1, '}');

    // Test with no delimiters
    reset_output();
    paren('\0', 1, '\0');

    printf("paren test passed\n");
}

void test_brack() {
    printf("Testing brack function...\n");

    reset_output();
    brack(3, "top", "mid", "bot");

    assert(strstr(output_buffer, "top") != NULL);
    assert(strstr(output_buffer, "mid") != NULL);
    assert(strstr(output_buffer, "bot") != NULL);

    printf("brack test passed\n");
}

void test_diacrit() {
    printf("Testing diacrit function...\n");

    eht[1] = 10;
    ebase[1] = 5;
    int original_height = eht[1];

    // Test vector diacritical
    reset_output();
    diacrit(1, 'V');
    assert(eht[1] == original_height + VERT(1));

    // Test hat diacritical
    reset_output();
    diacrit(1, 'H');

    // Test tilde diacritical
    reset_output();
    diacrit(1, 'T');

    printf("diacrit test passed\n");
}

void test_move() {
    printf("Testing move function...\n");

    // Test forward movement
    reset_output();
    move(0, 250, "1");
    assert(yyval == "1");

    // Test upward movement
    reset_output();
    move(1, 150, "2");

    // Test backward movement
    reset_output();
    move(2, 300, "3");

    // Test downward movement
    reset_output();
    move(3, 200, "4");

    printf("move test passed\n");
}

void test_funny() {
    printf("Testing funny function...\n");

    // Test sigma symbol
    reset_output();
    funny('S');
    assert(yyval > 0);
    assert(eht[yyval] == VERT(2));
    assert(ebase[yyval] == 0);

    // Test union symbol
    reset_output();
    funny('U');

    // Test intersection symbol
    reset_output();
    funny('A');

    // Test pi symbol
    reset_output();
    funny('P');

    // Test integral symbol
    reset_output();
    funny('I');

    printf("funny test passed\n");
}

int main() {
    printf("Running tests for ne2.c functions...\n\n");

    test_fromto();
    test_paren();
    test_brack();
    test_diacrit();
    test_move();
    test_funny();

    printf("\nAll tests passed!\n");
    return 0;
}
