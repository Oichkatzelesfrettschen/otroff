#include "os_abstraction.h"

/*
 * Unix implementation of OS abstraction layer.
 * These functions simply forward to the corresponding
 * POSIX system calls.
 */

int os_open(const char *path, int flags, int mode) {
    return open(path, flags, mode);
}

ssize_t os_read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t os_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

int os_close(int fd) {
    return close(fd);
}

off_t os_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int os_unlink(const char *path) {
    return unlink(path);
}

int os_stat(const char *path, struct stat *buf) {
    return stat(path, buf);
}

FILE *os_fopen(const char *path, const char *mode) {
    return fopen(path, mode);
}

int os_fclose(FILE *file) {
    return fclose(file);
}

int os_fork(void) {
    return fork();
}

int os_exec(const char* path, char* const argv[]) {
    if (!path || !argv) {
        return -1;
    }
    return execv(path, argv);
}

int os_mkdir(const char* path, mode_t mode) {
    if (!path) {
        return -1;
    }
    return mkdir(path, mode);
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
    return setenv(name, value, overwrite);
}
