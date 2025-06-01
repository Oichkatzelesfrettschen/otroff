#pragma once
#include "../../roff/cxx17_scaffold.hpp" // ensure C++17 features

#include <cstdio> // Use C++ headers
#include <cstdlib>
#include <cstring>
#include <cerrno>
// System headers that don't have direct cxxx counterparts
#include <sys/types.h> // For mode_t, pid_t etc.
#include <sys/stat.h>  // For mode_t, struct stat
#include <unistd.h>    // For fork, execv, lseek, read, write, close, unlink
#include <fcntl.h>     // For open flags

/*
 * Cross-platform wrapper functions for basic system operations.
 * The implementation differs per platform but presents a
 * uniform C90 interface to the rest of the code base.
 */

[[nodiscard]] int os_open(const char *path, int flags, int mode);
[[nodiscard]] ssize_t os_read(int fd, void *buf, size_t count);
[[nodiscard]] ssize_t os_write(int fd, const void *buf, size_t count);
[[nodiscard]] int os_close(int fd);
[[nodiscard]] off_t os_lseek(int fd, off_t offset, int whence);
[[nodiscard]] int os_unlink(const char *path);
[[nodiscard]] int os_stat(const char *path, struct stat *buf);
[[nodiscard]] FILE *os_fopen(const char *path, const char *mode);
[[nodiscard]] int os_fclose(FILE *file);

// Ported function declarations from os_unix.h (now in C++)
[[nodiscard]] int os_fork();
[[nodiscard]] int os_exec(const char* path, char* const argv[]);
[[nodiscard]] int os_mkdir(const char* path, mode_t mode);
[[nodiscard]] const char* os_get_error();
[[nodiscard]] int os_get_errno();
[[nodiscard]] char* os_getenv(const char* name);
[[nodiscard]] int os_setenv(const char* name, const char* value, int overwrite);
