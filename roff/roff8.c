/* Global data inspired by the original roff8.s.  Only a very small
 * subset is represented here for demonstration purposes. */

int slow = 1;
int pto = 9999;
int po = 0;
int ls = 1;
int ls1 = 1;
int pn = 1;
int ad = 1;
int fi = 1;
int ce = 0;
int in = 0;
int un = 0;
int ll = 65;
int pl = 66;
int skip = 0;
int ul = 0;
int hx = 1;
int hyf = 1;
int ohc = 200;
int tabc = '\'';
int cc = '.';

char ttyx[] = "/dev/tty0";
char bfn[] = "/tmp/rtma";

/* tables used by the hyphenation code */
extern char bxh[];
extern char hxx[];
extern char bxxh[];
extern char xhx[];
extern char xxh[];
