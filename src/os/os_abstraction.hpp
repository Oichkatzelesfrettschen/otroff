#pragma once
#include "cxx23_scaffold.hpp" // ensure C++23 features

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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
