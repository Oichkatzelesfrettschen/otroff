#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Helpers from runtime.c */
void mesg(int enable);
int dsp(int column);
void flush_output(char *buf, size_t *p);

/*
 * More complete translation of the initial set-up logic from roff1.s.
 * Only a subset of the original behaviour is reproduced.  The goal is
 * to demonstrate how the PDP-11 assembly initialisation maps onto
 * portable C89 code.
 */

/* suffix table loaded from "suffil" if available */
static unsigned short suftab[26];

/* command line flags */
static int stop_flag;
static int slow = 1;
static int pfrom;
static int pto;

/* temporary buffer file */
static char tmp_name[] = "roffbufXXXXXX";
static int tmp_fd = -1;

static void cleanup(int sig) {
  (void)sig;
  mesg(1);
  if (tmp_fd != -1) {
    close(tmp_fd);
    unlink(tmp_name);
  }
  exit(0);
}

/* create temporary buffer file similar to makebf routine */
static void makebf(void) {
  tmp_fd = mkstemp(tmp_name);
  if (tmp_fd == -1) {
    perror("mkstemp");
    exit(1);
  }
}

/* load the hyphenation suffix table if the file exists */
static void load_suffixes(void) {
  int fd = open("suffil", O_RDONLY);
  if (fd == -1)
    return;
  read(fd, suftab, sizeof(suftab));
  close(fd);
}

/* parse command line options translating the original argument logic */
static void parse_args(int argc, char **argv) {
  int i;
  for (i = 1; i < argc; ++i) {
    char *a = argv[i];
    if (a[0] == '+') {
      pfrom = atoi(a + 1);
      continue;
    }
    if (a[0] == '-') {
      if (a[1] == 's') {
        stop_flag = 1;
        continue;
      }
      if (a[1] == 'h') {
        slow = 0;
        continue;
      }
      pto = atoi(a + 1);
      continue;
    }
  }
}

int main(int argc, char **argv) {
  mesg(0);
  signal(SIGINT, cleanup);
  signal(SIGQUIT, cleanup);

  makebf();
  load_suffixes();

  if (argc <= 1)
    cleanup(0);
  parse_args(argc, argv);

  /* build identity translation table */
  unsigned char trtab[128];
  int i;
  for (i = 0; i < 128; ++i)
    trtab[i] = (unsigned char)i;

  /* simple stand-in for the real processing loop */
  char buf[256];
  size_t pos = 0;
  int c;
  while ((c = getchar()) != EOF) {
    buf[pos++] = (char)c;
    if (c == '\n' || pos >= sizeof(buf))
      flush_output(buf, &pos);
  }
  flush_output(buf, &pos);

  cleanup(0);
  return 0;
}
