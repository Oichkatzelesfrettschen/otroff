#ifndef CROFF_PROTO_H
#define CROFF_PROTO_H

/* Function prototypes for cross troff utilities */
char *setbrk(int size);
void init1(char a);
void init2(void);
void prstr(const char *s);
void prstrfl(const char *s);
#endif /* CROFF_PROTO_H */

