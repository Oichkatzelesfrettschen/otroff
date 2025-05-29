#ifndef CROFF_PROTO_H
#define CROFF_PROTO_H

/* Function prototypes for cross troff utilities */
char *setbrk(int size);
void init1(char a);
void init2(void);
int tatoi(void);
void setn1(int i);
int findr(int i);
void caserr(void);
void casenr(void);
void caseaf(void);
int vnumb(int i);
int hnumb(int i);
int inumb(int *n);
int quant(int n, int m);
int findn(int i);
void casetm(int x);
void casesp(int a);
void caseif(int x);
int rdtty(void);
void prstr(const char *s);
void prstrfl(const char *s);
#endif /* CROFF_PROTO_H */
