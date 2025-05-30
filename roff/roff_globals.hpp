#pragma once

#include "roff.h"
#include "os_abstraction.h"
#include <cstddef>

// Global variables defined in roff8.c
extern int ad; // Adjust mode flag
extern int fi; // Fill mode flag
extern int ce; // Center lines count
extern int ls; // Line spacing
extern int ls1; // Saved line spacing
extern int in; // Indent value
extern int un; // Temporary indent
extern int ll; // Line length
extern int pl; // Page length
extern int pn; // Page number
extern int skip; // Skip lines count
extern int ul; // Underline count
extern int nl; // Current line number
extern int nn; // Line numbering skip
extern int ni; // Line number indent
extern int po; // Page offset
extern int ma1; // Margin 1
extern int ma2; // Margin 2
extern int ma3; // Margin 3
extern int ma4; // Margin 4
extern int numbmod; // Line numbering mode
extern int lnumber; // Line number
extern int jfomod; // Justification mode
extern int ro; // Read-only mode
extern int nx; // Next file flag
extern int hx; // Header/footer flag
extern int hyf; // Hyphenation flag
extern int ohc; // Output hyphenation character
extern int tabc; // Tab character
extern int nlflg; // Newline flag
extern int ch; // Current character
extern int skp; // Skip processing flag
extern int ip; // Include pointer
extern int nextb; // Next buffer pointer
extern char cc; // Control character
extern char *ehead; // Even header
extern char *efoot; // Even footer
extern char *ohead; // Odd header
extern char *ofoot; // Odd footer
extern char nextf[]; // Next file name buffer
extern char bname[]; // Buffer name
extern unsigned char trtab[]; // Translation table
extern unsigned char tabtab[]; // Tab table
extern int ilist[]; // Include list
extern int *ilistp; // Include list pointer

// Functions shared across modules
void rbreak();
void eject();
void need(int lines);
void need2(int lines);
void nlines(int count, int spacing);
void topbot();
void skipcont();
void flushi();
int getchar_roff();
void putchar_roff(int c);
void storeline(int c);
int min(int value);
int number(int default_val);
int number1(int default_val);
void text();
void headin(char **header_ptr);
void getname(char *name_buffer);
void copyb();
int nextfile();

// Control command handlers implemented in roff2.cpp
void case_ad();
void case_br();
void case_cc();
void case_ce();
void case_ds();
void case_fi();
void case_in();
void case_ix();
void case_li();
void case_ll();
void case_ls();
void case_na();
void case_ne();
void case_nf();
void case_pa();
void case_bp();
void case_bl();
void case_pl();
void case_sk();
void case_sp();
void case_ss();
void case_tr();
void case_ta();
void case_ti();
void case_ul();
void case_un();
void case_hx();
void case_he();
void case_fo();
void case_eh();
void case_oh();
void case_ef();
void case_of();
void case_m1();
void case_m2();
void case_m3();
void case_m4();
void case_hc();
void case_tc();
void case_hy();
void case_n1();
void case_n2();
void case_nn();
void case_ni();
void case_jo();
void case_ar();
void case_ro();
void case_nx();
void case_po();
void case_de();
void case_ig();
void case_mk();
