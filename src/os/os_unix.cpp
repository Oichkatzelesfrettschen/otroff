#include "cxx23_scaffold.hpp"
#include "os_abstraction.hpp" // updated header extension

/*
 * Unix implementation of OS abstraction layer.
 * These functions simply forward to the corresponding
 * POSIX system calls.
 */

[[nodiscard]] int os_open(const char *path, int flags, int mode) {
    return open(path, flags, mode); // forward to POSIX open
}

[[nodiscard]] ssize_t os_read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

[[nodiscard]] ssize_t os_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

[[nodiscard]] int os_close(int fd) {
    return close(fd);
}

[[nodiscard]] off_t os_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence); // reposition file offset
}

[[nodiscard]] int os_unlink(const char *path) {
    return unlink(path);
}

[[nodiscard]] int os_stat(const char *path, struct stat *buf) {
    return stat(path, buf);
}

[[nodiscard]] FILE *os_fopen(const char *path, const char *mode) {
    return fopen(path, mode);
}

[[nodiscard]] int os_fclose(FILE *file) {
    return fclose(file);
}
