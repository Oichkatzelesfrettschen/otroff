/**
 * @file os_unix.c
 * @brief Unix Operating System Interface
 * @author Modern C++17 Engineering Team  
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/**
 * @brief Unix file operations
 */
int os_open_file(const char* filename, int flags) {
    if (!filename) return -1;
    return open(filename, flags, 0644);
}

int os_close_file(int fd) {
    return close(fd);
}

ssize_t os_read_file(int fd, void* buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t os_write_file(int fd, const void* buf, size_t count) {
    return write(fd, buf, count);
}

/**
 * @brief Unix process operations
 */
int os_fork(void) {
    return fork();
}

int os_exec(const char* path, char* const argv[]) {
    if (!path || !argv) return -1;
    return execv(path, argv);
}

/**
 * @brief Unix file system operations
 */
int os_stat(const char* path, struct stat* buf) {
    if (!path || !buf) return -1;
    return stat(path, buf);
}

int os_mkdir(const char* path, mode_t mode) {
    if (!path) return -1;
    return mkdir(path, mode);
}

/**
 * @brief Unix error handling
 */
const char* os_get_error(void) {
    return strerror(errno);
}

int os_get_errno(void) {
    return errno;
}

/**
 * @brief Unix environment operations
 */
char* os_getenv(const char* name) {
    if (!name) return NULL;
    return getenv(name);
}

int os_setenv(const char* name, const char* value, int overwrite) {
    if (!name || !value) return -1;
    return setenv(name, value, overwrite);
}