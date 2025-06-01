#include "../../cxx17_scaffold.hpp" // ensure C++17 features
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

// Ported functions from os_unix.c
#include <unistd.h> // For fork, execv, etc.
// <sys/types.h> is already included via os_abstraction.hpp (transitively or directly)
// <sys/stat.h> is already included via os_abstraction.hpp (transitively or directly)
#include <cstdlib>  // For getenv, setenv
#include <cstring>  // For strerror
#include <cerrno>   // For errno

[[nodiscard]] int os_fork() {
    return fork();
}

[[nodiscard]] int os_exec(const char* path, char* const argv[]) {
    if (!path || !argv) return -1; // Basic validation
    return execv(path, argv);
}

[[nodiscard]] int os_mkdir(const char* path, mode_t mode) {
    if (!path) return -1; // Basic validation
    return mkdir(path, mode);
}

[[nodiscard]] const char* os_get_error() {
    return strerror(errno);
}

[[nodiscard]] int os_get_errno() {
    return errno;
}

[[nodiscard]] char* os_getenv(const char* name) {
    if (!name) return nullptr; // Use nullptr
    return getenv(name);
}

[[nodiscard]] int os_setenv(const char* name, const char* value, int overwrite) {
    if (!name || !value) return -1; // Basic validation
    return setenv(name, value, overwrite);
}
