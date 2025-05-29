/* Global data inspired by the original roff8.s.  Only a very small
 * subset is represented here for demonstration purposes. */

int slow = 1;
int pto  = 9999;
int po   = 0;
int ls   = 1;
int ls1  = 1;
int pn   = 1;

char ttyx[] = "/dev/tty0";
char bfn[]  = "/tmp/rtma";

/* tables used by the hyphenation code */
extern char bxh[];
extern char hxx[];
extern char bxxh[];
extern char xhx[];
extern char xxh[];
