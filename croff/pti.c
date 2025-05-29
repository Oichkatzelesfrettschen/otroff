#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Portable C implementation of the historic phototypesetter
 * interpreter originally written in PDP-11 assembly (pti.s).
 * The program reads commands produced by troff and prints a
 * textual representation of the operations.  Only the control
 * flow of the entry routine is translated here.
 */

static int leadtot;  /* accumulated lead movement */
static int leadmode; /* direction flag for leads    */
static int caseflag; /* case conversion mode        */
static int esc;      /* current escape offset       */
static int esct;     /* total escape movement       */
static int escd;     /* non-zero if in reverse mode */

/*
 * Output a numeric value using the specified base.  This mirrors the
 * original \"numb\" subroutine from the assembly source.
 */
static void put_number(long n, int base) {
  char buf[32];
  char *p = &buf[31];
  const char digits[] = "0123456789";

  *p = '\0';
  if (n == 0) {
    *--p = '0';
  } else {
    unsigned long v = (n < 0) ? -n : n;
    while (v) {
      *--p = digits[v % (unsigned)base];
      v /= (unsigned)base;
    }
    if (n < 0)
      *--p = '-';
  }
  fputs(p, stdout);
}

/* Print a string literal.  Mimics the \"str\" subroutine. */
static void put_string(const char *s) { fputs(s, stdout); }

/* Output a control byte in octal form followed by a space. */
static void print_control(int c) {
  put_number((unsigned char)c, 8);
  put_string(" ");
}

int main(int argc, char **argv) {
  long offset = 0; /* optional seek offset */
  const char *filename = NULL;
  FILE *fp;

  /* Parse optional leading offset argument (octal). */
  if (argc > 1 && argv[1][0] == '-') {
    offset = strtol(argv[1] + 1, NULL, 8);
    --argc;
    ++argv;
  }

  if (argc > 1)
    filename = argv[1];

  fp = filename ? fopen(filename, "rb") : stdin;
  if (!fp) {
    perror(filename);
    return 1;
  }

  if (offset)
    fseek(fp, offset, SEEK_SET);

  for (;;) {
    int ch = fgetc(fp);
    if (ch == EOF)
      break;

    /* Negative bytes accumulate escape distance. */
    if (ch & 0x80) {
      print_control(ch);
      esc += (~ch) & 0x7F;
      continue;
    }

    /* If an escape was pending, announce it. */
    if (esc) {
      put_string(escd ? "\\< " : "\\> ");
      put_number(esc, 10);
      put_string("\n");
      if (escd)
        esc = -esc;
      esct += esc;
      esc = 0;
    }

    /* Display the plain character or control command. */
    print_control(ch);
    switch (ch) {
    case 100:
      put_string("Initialize\n");
      break;
    case 101:
      put_string("Lower Rail\n");
      break;
    case 102:
      put_string("Upper Rail\n");
      break;
    case 103:
      put_string("Upper Mag\n");
      break;
    case 104:
      put_string("Lower Mag\n");
      break;
    case 105:
      put_string("Lower Case\n");
      caseflag = 0;
      break;
    case 106:
      put_string("Upper Case\n");
      caseflag = 100;
      break;
    case 107:
      put_string("\\> mode, ");
      put_number(esct, 10);
      put_string("\n");
      escd = 0;
      break;
    case 110:
      put_string("\\< mode, ");
      put_number(esct, 10);
      put_string("\n");
      escd = 1;
      break;
    case 111:
      put_string("*****Stop*****\n");
      break;
    case 112:
      put_string("Lead forward, ");
      put_number(leadtot, 10);
      put_string("\n");
      leadmode = 0;
      break;
    case 114:
      put_string("Lead backward, ");
      put_number(leadtot, 10);
      put_string("\n");
      leadmode = 1;
      break;
    default:
      put_string("\n");
      break;
    }
  }

  put_string("Lead total ");
  put_number(leadtot, 10);
  put_string("\n");

  if (fp != stdin)
    fclose(fp);

  return 0;
}
