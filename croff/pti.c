#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Portable translation of the historic phototypesetter interpreter
 * (pti.s).  Labels from the original assembly source are preserved
 * as comments.
 * The entry point corresponds to the `start` label in the assembly.
 */

/* .bss */
static int leadtot; /* leadtot:  accumulated lead distance */
static int leadmode; /* leadmode: lead direction flag      */
static int caseflag; /* case:     case conversion offset   */
static int esc; /* esc:      pending escape distance  */
static int esct; /* esct:     total escape distance    */
static int escd; /* escd:     reverse escape mode      */
static int pts = 12; /* pts:      current point size       */

/* stab: type size table */
static const unsigned char stab[][2] = {
    {6, 10}, {7, 0}, {8, 1}, {9, 7}, {10, 2}, {11, 3}, {12, 4}, {14, 5}, {16, 11}, {18, 6}, {20, 12}, {22, 13}, {24, 14}, {28, 15}, {36, 16}, {0, 0}};

/* wtab: width/code table from pti.s */
struct wentry {
    unsigned char w;
    unsigned char c;
};
static const struct wentry wtab[] = {
    {15, 0}, {9, 145}, {0, 0}, {27, 153}, {18, 155}, {27, 53}, {28, 50}, {9, 150}, {9, 132}, {9, 133}, {16, 122}, {27, 143}, {9, 47}, {27, 123}, {9, 44}, {12, 43}, {18, 110}, {18, 111}, {18, 112}, {18, 113}, {18, 114}, {18, 115}, {18, 116}, {18, 117}, {18, 120}, {18, 121}, {9, 142}, {9, 23}, {0, 0}, {27, 140}, {0, 0}, {16, 147}, {36, 131}, {29, 103}, {23, 75}, {26, 70}, {29, 74}, {25, 72}, {24, 101}, {30, 65}, {29, 60}, {13, 66}, {16, 105}, {29, 107}, {24, 63}, {35, 62}, {30, 61}, {27, 57}, {22, 67}, {27, 55}, {28, 64}, {18, 76}, {24, 56}, {29, 106}, {28, 71}, {36, 104}, {28, 102}, {28, 77}, {24, 73}, {9, 134}, {0, 0}, {9, 135}, {0, 0}, {0, 0}, {0, 0}, {17, 25}, {19, 12}, {16, 27}, {20, 11}, {17, 31}, {13, 14}, {17, 45}, {21, 1}, {10, 6}, {10, 15}, {20, 17}, {10, 5}, {32, 4}, {21, 3}, {19, 33}, {20, 21}, {19, 42}, {14, 35}, {15, 10}, {12, 2}, {20, 16}, {20, 37}, {27, 41}, {20, 13}, {19, 51}, {16, 7}, {0, 0}, {5, 151}, {0, 0}, {0, 0}, {0, 0}};

/* ---------------------------------------------------------------------- */

/* prn: print control byte in octal followed by a space */
static void prn(int c) {
    printf("%o ", (unsigned char)c);
}

/* str: write literal string */
static void str(const char *s) {
    fputs(s, stdout);
}

/* numb: output number in given base */
static void numb(long n, int base) {
    char buf[32];
    char *p = &buf[31];
    int neg = 0;

    if (n < 0) {
        neg = 1;
        n = -n;
    }
    *p = '\0';
    do {
        *--p = '0' + (int)(n % base);
        n /= base;
    } while (n);
    if (neg)
        *--p = '-';
    fputs(p, stdout);
}

/* ---------------------------------------------------------------------- */

int main(int argc, char **argv) {
    /* argc: parsed below */
    long offset = 0; /* optional seek offset */
    const char *name = NULL;
    FILE *fp;

    /* argument parsing -- corresponds to start of pti.s */
    if (argc > 1 && argv[1][0] == '-') {
        offset = strtol(argv[1] + 1, NULL, 8); /* atoi */
        --argc;
        ++argv;
    }
    if (argc > 1)
        name = argv[1];

    fp = name ? fopen(name, "rb") : stdin; /* fopen */
    if (!fp) {
        perror(name);
        return 1;
    }

    if (offset)
        fseek(fp, offset, SEEK_SET); /* loop0 */

    /* loop: main interpreter loop */
    for (;;) {
        int ch = getc(fp);
        if (ch == EOF)
            break; /* ex */

        if (ch & 0200) { /* escape sequence byte */
            prn(ch);
            esc += (~ch) & 0177;
            continue;
        }

        if (esc) { /* flush pending escape */
            str(escd ? "\\< " : "\\> ");
            numb(esc, 10);
            str("\n");
            if (escd)
                esc = -esc;
            esct += esc;
            esc = 0;
        }

        prn(ch);

        /* command bytes 100-114 */
        switch (ch) {
        case 0100: /* minit */
            str("Initialize\n");
            continue;
        case 0101: /* mlr */
            str("Lower Rail\n");
            continue;
        case 0102: /* mur */
            str("Upper Rail\n");
            continue;
        case 0103: /* mum */
            str("Upper Mag\n");
            continue;
        case 0104: /* mlm */
            str("Lower Mag\n");
            continue;
        case 0105: /* mlc */
            str("Lower Case\n");
            caseflag = 0;
            continue;
        case 0106: /* muc */
            str("Upper Case\n");
            caseflag = 0100;
            continue;
        case 0107: /* mef */
            str("\\> mode, ");
            numb(esct, 10);
            str("\n");
            escd = 0;
            continue;
        case 0110: /* meb */
            str("\\< mode, ");
            numb(esct, 10);
            str("\n");
            escd = 1;
            continue;
        case 0111: /* mstop */
            str("*****Stop*****\n");
            continue;
        case 0112: /* mlf */
            str("Lead forward, ");
            numb(leadtot, 10);
            str("\n");
            leadmode = 0;
            continue;
        case 0114: /* mlb */
            str("Lead backward, ");
            numb(leadtot, 10);
            str("\n");
            leadmode = 1;
            continue;
        default:
            break;
        }

        /* miscellaneous commands */
        if ((ch & 0360) == 0100) { /* milgl */
            str("Illegal control\n");
            continue;
        }
        if ((ch & 0340) == 0140) { /* mlead */
            int n = (~ch) & 037;
            str("Lead ");
            numb(n, 10);
            str("\n");
            if (leadmode)
                n = -n;
            leadtot += n;
            continue;
        }
        if ((ch & 0360) == 0120) { /* msize */
            int n = ch & 017;
            size_t i;

            str("Size ");
            for (i = 0; stab[i][0]; ++i) {
                if (stab[i][1] == n) {
                    pts = stab[i][0];
                    numb(pts, 10);
                    break;
                }
            }
            str("\n");
            continue;
        }
        if ((ch & 0300) == 0) { /* printable character */
            unsigned char c = ch + caseflag;
            const struct wentry *w = NULL;
            size_t i;

            for (i = 0; i < sizeof(wtab) / sizeof(wtab[0]); ++i) {
                if (wtab[i].c == c) {
                    w = &wtab[i];
                    c = 040 + i; /* map back to ASCII */
                    break;
                }
            }
            int n = 0;
            if (w)
                n = (w->w * pts) / 6;
            putchar(c);
            str("\n");
            (void)n; /* width not otherwise used in this translation */
            continue;
        }

        str("\n");
    }

    /* ex: */
    str("Lead total ");
    numb(leadtot, 10);
    str("\n");

    if (fp != stdin)
        fclose(fp);
    return 0;
}
