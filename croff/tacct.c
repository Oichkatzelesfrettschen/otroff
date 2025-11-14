/* C17 - no scaffold needed */
/*
 * tacct.c - Paper usage accounting utility for troff
 * 
 * This program reads accounting data and reports paper usage per user.
 * It reads from /etc/passwd to map UIDs to usernames and from accounting
 * data files to calculate paper consumption in feet.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // POSIX header
#include <string.h>

/* Maximum number of users supported */
#define MAX_USERS 256
/* Maximum line length for passwd entries */
#define MAX_LINE 100
/* Maximum date string length */
#define MAX_DATE 40
/* Conversion factor: square inches to square feet (1728 = 12*12*12) */
#define INCHES_TO_FEET 1728.0

/* Paper usage per user (indexed by UID) */
static float paper[MAX_USERS];

/* Local passwd structure definition */
struct passwd {
    char *pw_name; /* username */
    char *pw_passwd; /* password (encrypted) */
    int pw_uid; /* user ID */
    int pw_gid; /* group ID */
    char *pw_gecos; /* real name */
    char *pw_dir; /* home directory */
    char *pw_shell; /* shell */
};

/* File pointers for passwd and accounting data */
static FILE *passwd_fp = NULL;
static FILE *acct_fp = NULL;

/* Buffer for reading accounting records */
static int acct_record[3];

/* Default path to accounting data */
static char acctname[] = "/usr/actg/data/troffactg";

/* Function prototypes */
static struct passwd *getpwent(void);
static char *pwskip(char *ap);
static int getn(void *p, int n);
static void cleanup_and_exit(int status);
static void reset_passwd_file(void);

/*
 * Entry point for paper accounting utility.
 * 
 * Usage: tacct [device_char]
 * 
 * The optional device_char parameter modifies the accounting file path
 * by replacing the 10th character of the default path.
 */
int main(int argc, char **argv) {
    register int i;
    int last_uid;
    register struct passwd *p;
    char date[MAX_DATE];
    float total;

    /* Initialize paper usage array */
    for (i = 0; i < MAX_USERS; i++) {
        paper[i] = 0.0;
    }

    /* Open passwd file */
    passwd_fp = fopen("/etc/passwd", "r");
    if (passwd_fp == NULL) {
        printf("Cannot open /etc/passwd\n");
        cleanup_and_exit(1);
    }

    /* Handle optional device character argument */
    if (argc == 2) {
        acctname[9] = argv[1][0];
    }

    /* Open accounting data file */
    acct_fp = fopen(acctname, "r");
    if (acct_fp == NULL) {
        printf("Cannot open: %s\n", acctname);
        cleanup_and_exit(1);
    }

    /* Read date line from accounting file */
    i = 0;
    while (i < MAX_DATE - 1) {
        int c = getc(acct_fp);
        if (c == EOF) {
            printf("Error reading date from accounting file\n");
            cleanup_and_exit(1);
        }
        date[i] = ((char)c);
        if (date[i] == '\n') {
            break;
        }
        i++;
    }
    date[i] = '\0';

    /* Process accounting records */
    total = 0.0;
    last_uid = -1;

    while (getn(acct_record, sizeof(acct_record)) == sizeof(acct_record)) {
        int uid = acct_record[1];
        float usage;

        /* Validate UID range */
        if (uid < 0 || uid >= MAX_USERS) {
            continue;
        }

        /* Convert paper usage from square inches to feet */
        usage = static_cast<float>(acct_record[0]) / INCHES_TO_FEET;
        paper[uid] += usage;
        total += usage;
        last_uid = uid;
    }

    /* Print header */
    printf("%s", date);
    printf("UID\tFeet of paper\n");

    /* Print usage for each user with non-zero consumption */
    for (i = 0; i < MAX_USERS; i++) {
        if (paper[i] != 0.0) {
            reset_passwd_file();

            /* Find username for this UID */
            while ((p = getpwent()) != NULL) {
                if (p->pw_uid == i) {
                    /* Mark last user with underscore */
                    if (last_uid == p->pw_uid) {
                        printf("_\b");
                    }
                    printf("%s\t%8.1f\n", p->pw_name, paper[i]);
                    break;
                }
            }

            /* If no username found, print UID number */
            if (p == NULL) {
                printf("%d\t%8.1f\n", i, paper[i]);
            }
        }
    }

    /* Print total usage */
    printf("Total\t%8.1f\n", total);

    cleanup_and_exit(0);
    return 0; /* Never reached, but satisfies compiler */
}

/*
 * Read the next entry from /etc/passwd.
 * 
 * Returns: Pointer to static passwd structure, or NULL on EOF/error
 */
static struct passwd *getpwent(void) {
    register char *p;
    register int c;
    static struct passwd passwd_entry;
    static char line[MAX_LINE];

    if (passwd_fp == NULL) {
        return NULL;
    }

    /* Read one line from passwd file */
    p = line;
    while ((c = getc(passwd_fp)) != '\n') {
        if (c == EOF) {
            return NULL;
        }
        if (p < line + MAX_LINE - 1) {
            *p++ = ((char)c);
        }
    }
    *p = '\0';

    /* Parse passwd line fields separated by ':' */
    p = line;
    passwd_entry.pw_name = p;
    p = pwskip(p);
    passwd_entry.pw_passwd = p;
    p = pwskip(p);
    passwd_entry.pw_uid = atoi(p);
    p = pwskip(p);
    passwd_entry.pw_gid = atoi(p);
    p = pwskip(p);
    passwd_entry.pw_gecos = p;
    p = pwskip(p);
    passwd_entry.pw_dir = p;
    p = pwskip(p);
    passwd_entry.pw_shell = p;

    return &passwd_entry;
}

/*
 * Advance pointer past a field separated by ':'
 * 
 * Parameters:
 *   ap - pointer to current position in string
 * 
 * Returns: pointer to start of next field
 */
static char *pwskip(char *ap) {
    register char *p;

    p = ap;
    while (*p != ':' && *p != '\0') {
        p++;
    }

    if (*p == ':') {
        *p++ = '\0';
    }

    return p;
}

/*
 * Read up to n bytes from accounting file.
 * 
 * Parameters:
 *   p - buffer to read into
 *   n - number of bytes to read
 * 
 * Returns: number of bytes actually read
 */
static int getn(void *p, int n) {
    register int i;
    register char *cp = ((char*)p);

    if (acct_fp == NULL) {
        return 0;
    }

    for (i = 0; i < n; i++) {
        int c = getc(acct_fp);
        if (c == EOF) {
            break;
        }
        cp[i] = ((char)c);
    }

    return i;
}

/*
 * Reset passwd file pointer to beginning
 */
static void reset_passwd_file(void) {
    if (passwd_fp != NULL) {
        rewind(passwd_fp);
    }
}

/*
 * Clean up resources and exit
 * 
 * Parameters:
 *   status - exit status code
 */
static void cleanup_and_exit(int status) {
    if (passwd_fp != NULL) {
        fclose(passwd_fp);
    }
    if (acct_fp != NULL) {
        fclose(acct_fp);
    }
    exit(status);
}
