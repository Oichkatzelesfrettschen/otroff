/* C17 - no scaffold needed */
/*
 * n2.c - Character output processing and program termination for troff (C90)
 *
 * Copyright (c) 1979 Bell Telephone Laboratories, Incorporated
 * Copyright (c) 2024 Modern C90 port
 *
 * This module handles the core character output processing, device output
 * buffering, and multi-stage program termination routines for the troff
 * text formatting system. It provides the primary interface between the
 * formatted text processing engine and the output device drivers.
 *
 * MAJOR FUNCTIONS:
 *   - Character processing and translation (pchar, pchar1)
 *   - Output buffer management (oput, oputs, flusho)
 *   - Multi-stage program termination (done, done1, done2, done3)
 *   - Device-specific output handling
 *   - Pipeline management for NROFF mode
 *   - Usage accounting and cleanup
 *
 * DESIGN PRINCIPLES:
 *   1. Separation of character processing from device output
 *   2. Buffered output for efficiency
 *   3. Multi-stage termination for proper cleanup
 *   4. Support for both NROFF and TROFF output modes
 *   5. Graceful error handling and resource cleanup
 *
 * CHARACTER PROCESSING PIPELINE:
 *   Input character -> pchar() -> translation -> pchar1() -> device output
 *
 * TERMINATION SEQUENCE:
 *   done() -> done1() -> done2() -> done3() -> exit()
 *
 * COMPATIBILITY:
 *   - C90 compliant with POSIX extensions
 *   - Supports both ASCII and binary output modes
 *   - Compatible with original Bell Labs troff behavior
 *   - Maintains historical escape sequence processing
 */

#include "tdef.h" // troff definitions
#include "env.h" // environment data
#include "t.h" // troff common header
#include "troff_processor.h" // processor state

#include <stdlib.h> /* C90: exit, malloc, free */
#include <unistd.h> /* POSIX: write, close, open, sleep */
#include <signal.h> /* C90: signal, SIG_DFL, SIGINT, SIGTERM */
#include <fcntl.h> /* POSIX: open flags O_WRONLY, etc. */
#include <stdio.h> /* C90: standard I/O functions */
#include <sys/types.h> /* POSIX: pid_t, uid_t */
#include <sys/wait.h> /* POSIX: wait functions */

/* External variables from other modules */
extern TroffProcessor g_processor; /* Shared processor state */
extern int dilev; /* Diversion level */
extern struct env *dip; /* Current diversion pointer */
extern int eschar; /* Escape character */
extern int tlss; /* Temporary line spacing storage */
extern int tflg; /* Test flag */
extern int ascii; /* ASCII output mode flag */
extern int print; /* Print mode flag */
extern char trtab[]; /* Character translation table */
extern int waitf; /* Wait flag counter */
extern char ptname[]; /* Phototypesetter device name */
extern int ptid; /* Phototypesetter file descriptor */
extern int offset; /* Current offset */
extern int em; /* End macro */
extern int ds; /* Diversion stack flag */
extern int ip; /* Input pointer */
extern int mflg; /* Macro flag */
extern int woff; /* Word offset flag */
extern int nflush; /* Number of flushes */
extern int lgf; /* Language flag */
extern int app; /* Append flag */
extern int nfo; /* No format output flag */
extern int donef; /* Done flag */
extern int *frame; /* Stack frame pointer */
extern int *stk; /* Stack base pointer */
extern int *pendw; /* Pending word pointer */
extern int nofeed; /* No feed flag */
extern int trap; /* Trap flag */
extern int ttys[3]; /* Terminal settings */
extern int quiet; /* Quiet mode flag */
extern int pendnf; /* Pending no-fill flag */
extern int ndone; /* Number done */
extern int lead; /* Leading value */
extern int ralss; /* Relative line spacing */
extern int paper; /* Paper usage counter */
extern int gflag; /* Global flag */
extern int *nxf; /* Next frame pointer */
extern char *unlkp; /* Unlink path pointer */
extern int ibf; /* Input buffer file descriptor */
extern char *nextf[]; /* Next file array */
extern int pipeflg; /* Pipeline flag */
extern int ejf; /* Eject flag */
extern int no_out; /* No output flag */
extern int level; /* Nesting level */
extern int xxx; /* Reserved variable */
/* d and v are defined in header files */
extern int chtab[]; /* Character table for special chars */

/* Global variables defined in this module */
int toolate; /* Too late flag for output */
int error; /* Error status accumulator */

#ifndef NROFF
extern int acctf; /* Accounting file descriptor */
#endif

/* Version identification */
static char Sccsid[] = "@(#)n2.c  1.8 of 5/13/77";

/* Function prototypes for C90 compliance */
void oput(int i);
void oputs(const char *i);
void flusho(void);
#ifdef NROFF
void casepi(void);
#endif

/* Forward declarations for functions defined in this file */
void pchar(int c);
void pchar1(int c);
int done(int x);
void done1(int x);
void done2(int x);
void done3(int x);
void edone(int x);

/* Forward declarations for external functions */
extern void wbf(int i);
extern void ptout(int i);
extern int control(int i, int j);
extern void reset(void);
extern void wbt(int i);
extern void getword(int i);
extern void tbreak(void);
extern void eject(int i);
extern void ptlead(void);
extern void twdone(void);
extern void mesg(int i);
extern void prstr(const char *s);
extern uid_t getuid(void);
extern long seek(int fd, long offset, int whence);
extern void stty(int fd, int *args);
extern int skip(void);
extern int getname(void);
extern int pipe(int *fd);
extern pid_t fork(void);
extern int dup(int fd);
extern int execl(const char *path, const char *arg0, ...);

#ifndef NROFF
static void report(void);
#endif

/*
 * pchar - Process a character for output
 * 
 * This function handles character processing including motion codes,
 * special characters, and translation table lookups before passing
 * the character to pchar1 for actual output.
 *
 * Parameters:
 *   c - Character code to process (may include motion bits)
 */
void pchar(int c) {
    register int i, j;

    /* Check if character includes motion information */
    if ((i = c) & MOT) {
        pchar1(i);
        return;
    }

    /* Extract character mask and process special cases */
    switch (j = i & CMASK) {
    case 0: /* Null character */
    case IMP: /* Impossible character */
    case RIGHT: /* Right motion */
    case LEFT: /* Left motion */
        return;

    case HX: /* Horizontal spacing */
        /* Calculate spacing value from tlss and character bits */
        j = (tlss >> 9) | ((i & ~0777) >> 3);
        if (i & 040000) {
            /* Baseline spacing */
            j &= ~(040000 >> 3);
            if (dip && j > dip->blss)
                dip->blss = j;
        } else {
            /* Above-baseline spacing */
            if (dip && j > dip->alss) {
                dip->alss = j;
                ralss = dip->alss;
            }
        }
        tlss = 0;
        return;

    case LX: /* Line spacing storage */
        tlss = i;
        return;

    case PRESC: /* Prescripted character */
        /* Use escape character if not in diversion output */
        if (!dip || !dip->op)
            j = eschar;
        /* Fall through to default case */
    default:
        /* Apply character translation and preserve non-character bits */
        if (j >= 0 && j < BMASK)
            i = (trtab[j] & BMASK) | (i & ~CMASK);
    }

    pchar1(i);
}

/*
 * pchar1 - Output a processed character
 * 
 * This function handles the actual output of characters, including
 * diversion buffering, test mode handling, and device-specific
 * character encoding.
 *
 * Parameters:
 *   c - Processed character code to output
 */
void pchar1(int c) {
    register int i, j, *k;

    j = (i = c) & CMASK;

    /* Handle diversion output */
    if (dip && dip->op) {
        wbf(i); /* Write to buffer */
        dip->op = offset;
        return;
    }

    /* Skip output in test mode or when print is disabled */
    if (!tflg && !print) {
        /* Reset line spacing on newline */
        if (j == '\n' && dip)
            dip->alss = dip->blss = 0;
        return;
    }

    /* Skip output if disabled or character is filler */
    if (no_out || (j == FILLER))
        return;

#ifndef NROFF
    /* Handle ASCII output mode */
    if (ascii) {
        /* Output space for motion characters */
        if (i & MOT) {
            oput(' ');
            return;
        }

        /* Output standard ASCII characters directly */
        if (j < 0177) {
            oput(i);
            return;
        }

        /* Handle special characters with ASCII substitutions */
        switch (j) {
        case 0200: /* Em dash variants */
        case 0210:
            oput('-');
            break;
        case 0211: /* fi ligature */
            oputs("fi");
            break;
        case 0212: /* fl ligature */
            oputs("fl");
            break;
        case 0213: /* ff ligature */
            oputs("ff");
            break;
        case 0214: /* ffi ligature */
            oputs("ffi");
            break;
        case 0215: /* ffl ligature */
            oputs("ffl");
            break;
        default:
            /* Look up character in special character table */
            for (k = chtab; *++k != j; k++)
                if (*k == 0)
                    return; /* Character not found */
            /* Output as \(xx escape sequence */
            oput('\\');
            oput('(');
            oput(*--k & BMASK);
            oput(*k >> BYTE);
        }
    } else
#endif
        /* Use device-specific output for non-ASCII mode */
        ptout(i);
}

/*
 * oput - Output a single character to the buffer
 * 
 * Adds a character to the output buffer and flushes if buffer is full.
 *
 * Parameters:
 *   i - Character to output
 */
void
oput(int i) {
    *g_processor.outputPtr++ = ((char)i);

    /* Flush buffer when full (accounting for ASCII mode differences) */
    if (g_processor.outputPtr ==
        (g_processor.outputBuffer + OBUFSZ + ascii - 1))
        flusho();
}

/*
 * oputs - Output a NUL-terminated string
 * 
 * Outputs each character of a string using oput().
 *
 * Parameters:
 *   i - Pointer to NUL-terminated string
 */
void
oputs(const char *i) {
    while (*i != '\0')
        oput(*i++);
}

/*
 * flusho - Flush the device output buffer
 * 
 * Writes the accumulated output buffer to the output device,
 * opening the device if necessary. Handles error conditions
 * gracefully and maintains buffer consistency.
 */
void
flusho(void) {
    ssize_t bytes_written;

    /* Add terminator for non-ASCII output */
    if (!ascii)
        *g_processor.outputPtr++ = '\0';

    /* Open output device if not already open */
    if (!ptid) {
        while ((ptid = open(ptname, O_WRONLY)) < 0) {
            if (++waitf <= 2)
                prstr("Waiting for Typesetter.\n");
            sleep(15);
        }
    }

    /* Write buffer to device unless output is disabled */
    if (no_out == 0) {
        bytes_written = write(ptid, g_processor.outputBuffer,
                              (size_t)(g_processor.outputPtr -
                                       g_processor.outputBuffer));
        if (bytes_written < 0) {
            toolate = -1; /* Mark write error */
        } else {
            toolate = (int)bytes_written;
        }
    }

    /* Reset buffer pointer */
    g_processor.outputPtr = g_processor.outputBuffer;
}

/*
 * done - Main termination function
 * 
 * Handles macro cleanup, diversion cleanup, and final output.
 * This function manages the orderly shutdown of the troff system.
 *
 * Parameters:
 *   x - Exit status to accumulate
 *
 * Returns:
 *   Exit status (though function may not return)
 */
int done(int x) {
    register int i;

    /* Accumulate error status */
    error |= x;
    level = 0;
    app = ds = lgf = 0;

    /* Handle end macro if present */
    if ((i = em) != 0) {
        donef = -1;
        em = 0;
        if (control(i, 0))
            reset();
    }

    /* Skip final processing if no format output */
    if (!nfo)
        done3(0);

    /* Reset state variables safely */
    mflg = 0;
    if (dip)
        dip = (struct env *)(&d[0]);

    /* Handle word breaks and pending operations */
    if (woff)
        wbt(0);
    if (pendw)
        getword(1);
    pendnf = 0;

    /* Handle first-time done processing */
    if (donef == 1)
        done1(0);
    donef = 1;

    /* Reset stack and frame pointers safely */
    ip = 0;
    if (stk) {
        frame = stk;
        nxf = frame + STKSIZE;
    }

    /* Final page break and cleanup */
    if (!ejf)
        tbreak();
    nflush++;
    eject(0);
    reset();

    return error;
}

/*
 * done1 - First stage of termination processing
 * 
 * Handles page ejection and leading setup before calling done2.
 *
 * Parameters:
 *   x - Exit status to accumulate
 */
void done1(int x) {
    /* Accumulate error status */
    error |= x;

    /* Eject current page if there's content */
    if (v.nl) {
        trap = 0;
        eject(0);
        reset();
    }

    /* Handle no-feed mode vs normal termination */
    if (nofeed) {
        ptlead();
        flusho();
        done3(0);
    } else {
        /* Add trailer unless global flag set */
        if (!gflag)
            lead += TRAILER;
        done2(0);
    }
}

/*
 * done2 - Second stage of termination processing
 * 
 * Handles device-specific termination sequences and calls done3.
 *
 * Parameters:
 *   x - Exit status to accumulate
 */
void done2(int x) {
    register int i;

    /* Output leading */
    ptlead();

#ifndef NROFF
    /* Output device termination sequence for non-ASCII mode */
    if (!ascii) {
        oput(T_INIT);
        oput(T_STOP);
        /* Add padding unless global flag set */
        if (!gflag)
            for (i = 8; i > 0; i--)
                oput(T_PAD);
    }
#endif

    /* Flush final output and complete termination */
    flusho();
    done3(x);
}

/*
 * done3 - Final stage of termination processing
 * 
 * Performs final cleanup including signal handling, file cleanup,
 * and program exit.
 *
 * Parameters:
 *   x - Exit status to accumulate
 */
void done3(int x) {
    /* Accumulate final error status */
    error |= x;

    /* Reset signal handlers to default */
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);

    /* Clean up temporary files */
    close(ibf);
    unlink(unlkp);

#ifdef NROFF
    /* NROFF-specific cleanup */
    twdone();
#endif

    /* Restore terminal settings if in quiet mode */
    if (quiet) {
        ttys[2] |= ECHO;
        stty(0, ttys);
    }

    /* Enable messages in ASCII mode */
    if (ascii)
        mesg(1);

#ifndef NROFF
    /* Generate usage report for troff */
    report();
#endif

    /* Exit with accumulated error status */
    exit(error);
}

/*
 * edone - Emergency termination
 * 
 * Resets stack state and calls normal termination.
 *
 * Parameters:
 *   x - Exit status
 */
void edone(int x) {
    /* Reset stack to initial state */
    frame = stk;
    nxf = frame + STKSIZE;
    ip = 0;

    /* Call normal termination */
    done(x);
}

#ifndef NROFF
/*
 * report - Generate usage accounting report
 * 
 * Writes paper usage information to the accounting file for troff.
 */
static void
report(void) {
    struct acct_record {
        int use; /* Paper usage count */
        char uid; /* User ID */
    } a;

    /* Write accounting record if output device used and paper consumed */
    if ((ptid != 1) && paper) {
        seek(acctf, 0, 2); /* Seek to end of accounting file */
        a.use = paper;
        a.uid = ((char)getuid());
        write(acctf, &a, sizeof(a));
        close(acctf);
    }
}
#endif

#ifdef NROFF
/*
 * casepi - Handle pipeline creation for NROFF
 * 
 * Creates a pipeline to an external program for NROFF output processing.
 * This function forks a child process and sets up pipes for communication.
 * Includes comprehensive error handling and resource cleanup.
 */
void
casepi(void) {
    register int i;
    int id[2]; /* Pipe file descriptors */
    pid_t child_pid; /* Child process ID */

    /* Check for error conditions */
    if (toolate || skip() || !getname() || (pipe(id) == -1)) {
        prstr("Pipe not created.\n");
        return;
    }

    /* Fork child process */
    if ((child_pid = fork()) == -1) {
        prstr("Pipe not created.\n");
        close(id[0]);
        close(id[1]);
        return;
    }

    /* Parent process setup */
    if (child_pid > 0) {
        ptid = id[1]; /* Use write end of pipe as output */
        close(id[0]); /* Close read end in parent */
        toolate++;
        pipeflg++;
        return;
    }

    /* Child process setup */
    close(0); /* Close stdin */
    if (dup(id[0]) == -1) {
        prstr("dup failed\n");
        exit(-4);
    }
    close(id[1]); /* Close pipe write end */
    close(id[0]); /* Close original read end */

    /* Execute the specified program */
    execl(nextf[0], nextf[0], NULL);

    /* Error handling if exec fails */
    prstr("Cannot exec: ");
    prstr(nextf[0]);
    prstr("\n");
    exit(-4);
}
#endif

/*
 * End of n2.c
 *
 * This module provides the core character output processing and program
 * termination functionality for the troff text formatting system. The
 * multi-stage termination sequence ensures proper cleanup of all resources
 * including output buffers, temporary files, and system state.
 *
 * Key design features:
 * - Buffered output for performance
 * - Character translation for device independence
 * - Graceful termination with proper cleanup
 * - Support for both ASCII and binary output modes
 * - Pipeline support for NROFF post-processing
 */
