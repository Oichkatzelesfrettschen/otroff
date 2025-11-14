#include "os_abstraction.h"

#ifdef _WIN32
#include <io.h>

/*
 * Windows implementation of OS abstraction layer.
 * These wrappers call the MSVCRT equivalents of POSIX
 * system calls.
 */

int os_open(const char *path, int flags, int mode) {
    return _open(path, flags, mode);
}

ssize_t os_read(int fd, void *buf, size_t count) {
    return _read(fd, buf, count);
}

ssize_t os_write(int fd, const void *buf, size_t count) {
    return _write(fd, buf, count);
}

int os_close(int fd) {
    return _close(fd);
}

off_t os_lseek(int fd, off_t offset, int whence) {
    return _lseek(fd, offset, whence);
}

int os_unlink(const char *path) {
    return _unlink(path);
}

int os_stat(const char *path, struct stat *buf) {
    return _stat(path, buf);
}

FILE *os_fopen(const char *path, const char *mode) {
    FILE *f = NULL;
    errno_t err = fopen_s(&f, path, mode);
    if (err != 0) {
        return NULL;
    }
    return f;
}

int os_fclose(FILE *file) {
    return fclose(file);
}

/* Stub implementations for process functions on Windows */
int os_fork(void) {
    /* fork() not available on Windows */
    errno = ENOSYS;
    return -1;
}

int os_exec(const char* path, char* const argv[]) {
    /* execv() not directly available on Windows, would need _execv */
    (void)path;
    (void)argv;
    errno = ENOSYS;
    return -1;
}

int os_mkdir(const char* path, mode_t mode) {
    /* Windows mkdir doesn't use mode parameter */
    (void)mode;
    return _mkdir(path);
}

const char* os_get_error(void) {
    return strerror(errno);
}

int os_get_errno(void) {
    return errno;
}

char* os_getenv(const char* name) {
    if (!name) {
        return NULL;
    }
    return getenv(name);
}

int os_setenv(const char* name, const char* value, int overwrite) {
    if (!name || !value) {
        return -1;
    }
    /* Windows _putenv_s always overwrites */
    (void)overwrite;
    return _putenv_s(name, value);
}

#endif /* _WIN32 */
