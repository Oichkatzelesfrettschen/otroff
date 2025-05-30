#include "cxx23_scaffold.hpp"
#include "os_abstraction.h"
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

[[nodiscard]] FILE *os_fopen(const char *path, const char *mode) {
    FILE *f = nullptr; // std pointer initialization
    fopen_s(&f, path, mode);
    return f;
}

[[nodiscard]] int os_fclose(FILE *file) {
    return fclose(file);
}
