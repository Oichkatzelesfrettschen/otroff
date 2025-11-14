#ifndef OS_ABSTRACTION_H
#define OS_ABSTRACTION_H

/* Pure C17 cross-platform OS abstraction layer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Cross-platform wrapper functions for basic system operations.
 * The implementation differs per platform but presents a
 * uniform C interface to the rest of the code base.
 */

/* File operations */
int os_open(const char *path, int flags, int mode);
ssize_t os_read(int fd, void *buf, size_t count);
ssize_t os_write(int fd, const void *buf, size_t count);
int os_close(int fd);
off_t os_lseek(int fd, off_t offset, int whence);
int os_unlink(const char *path);
int os_stat(const char *path, struct stat *buf);
FILE *os_fopen(const char *path, const char *mode);
int os_fclose(FILE *file);

/* Process and system operations */
int os_fork(void);
int os_exec(const char* path, char* const argv[]);
int os_mkdir(const char* path, mode_t mode);
const char* os_get_error(void);
int os_get_errno(void);
char* os_getenv(const char* name);
int os_setenv(const char* name, const char* value, int overwrite);

#ifdef __cplusplus
}
#endif

#endif /* OS_ABSTRACTION_H */
