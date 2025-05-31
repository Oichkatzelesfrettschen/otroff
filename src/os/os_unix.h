/**
 * @file os_unix.h
 * @brief Unix Operating System Interface Header
 * @author Modern C++17 Engineering Team  
 * @date 2024
 */

#ifndef OS_UNIX_H
#define OS_UNIX_H

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Unix file operations
 */
int os_open_file(const char *filename, int flags);
int os_close_file(int fd);
ssize_t os_read_file(int fd, void *buf, size_t count);
ssize_t os_write_file(int fd, const void *buf, size_t count);

/**
 * @brief Unix process operations
 */
int os_fork(void);
int os_exec(const char *path, char *const argv[]);

/**
 * @brief Unix file system operations
 */
int os_stat(const char *path, struct stat *buf);
int os_mkdir(const char *path, mode_t mode);

/**
 * @brief Unix error handling
 */
const char *os_get_error(void);
int os_get_errno(void);

/**
 * @brief Unix environment operations
 */
char *os_getenv(const char *name);
int os_setenv(const char *name, const char *value, int overwrite);

#ifdef __cplusplus
}
#endif

#endif /* OS_UNIX_H */