/*
 * env.h - troff environment diversion structure and interface
 *
 * This header defines the environment structure used for diversions in troff.
 * It is written in portable C90, with C++ compatibility, and is suitable for
 * inclusion in both C and C++ projects. All members are documented for clarity.
 */
#pragma once
#include "../cxx17_scaffold.hpp" // common utilities

/*
 * Environment diversion structure used by troff.
 *
 * Members:
 *   op     - Output position
 *   dnl    - Current vertical position
 *   dimac  - Diversion macro
 *   ditrap - Diversion trap
 *   ditf   - Diversion trap flag
 *   alss   - Amount of extra leading after line
 *   blss   - Amount of extra leading before line
 *   nls    - Indicates a line was output
 *   mkline - Line that should trigger a trap
 *   maxl   - Maximum line number seen
 *   hnl    - Line number of next trap
 *   curd   - Current diversion name
 */
typedef struct env {
    int op; /* output position */
    int dnl; /* current vertical position */
    int dimac; /* diversion macro */
    int ditrap; /* diversion trap */
    int ditf; /* diversion trap flag */
    int alss; /* amount of extra leading after line */
    int blss; /* amount of extra leading before line */
    int nls; /* indicates a line was output */
    int mkline; /* line that should trigger a trap */
    int maxl; /* maximum line number seen */
    int hnl; /* line number of next trap */
    int curd; /* current diversion name */
} env_t;

/*
 * Function prototypes for env operations.
 * Add new prototypes here as needed.
 * Example:
 *   void init_env(env_t *e);
 */

