#include "cxx23_scaffold.hpp"
/**
 * @file ne0.c
 * @brief Core initialization and utility module for the neqn preprocessor
 *
 * @details
 * This module provides core initialization routines and utility functions
 * for the neqn mathematical equation preprocessor. The original 1977 AT&T
 * implementation distributed functionality across multiple source files
 * (ne0.c through ne6.c). This modern C90-compliant version maintains
 * backward compatibility while adding enhanced error handling and
 * documentation.
 *
 * Historical Context:
 * - Originally part of UNIX Version 7 (1979)
 * - Based on the eqn preprocessor by Brian Kernighan and Lorinda Cherry
 * - Simplified version for terminal output (hence "neqn" = "new eqn")
 *
 * Module Responsibilities:
 * - System initialization and cleanup
 * - Global state management
 * - Utility functions used across the neqn system
 * - Error handling coordination
 *
 * @author Original: AT&T Bell Labs (1977)
 * @author Modern C90 port: Various contributors
 * @version 2.0 (C90 compliant)
 * @date 2023
 *
 * @copyright
 * Copyright (c) 1979 The Regents of the University of California.
 * Copyright (c) 2023 Modern contributors.
 * All rights reserved.
 *
 * This software was developed by the University of California, Berkeley
 * and its contributors. It may be freely redistributed under the terms
 * of the Berkeley Software Distribution license.
 */

/* ================================================================
 * SYSTEM INCLUDES - C90 Standard Library Headers
 * ================================================================ */
#include <stdio.h> /* Standard I/O operations */
#include <stdlib.h> /* Memory allocation, program control */
#include <string.h> /* String manipulation functions */
#include <ctype.h> /* Character classification */
#include <errno.h> /* Error number definitions */
#include <limits.h> /* Implementation-defined limits */
#include <assert.h> /* Assertion macros for debugging */
#include <signal.h> /* Signal handling */
#include <locale.h> /* Locale support */

/* ================================================================
 * PROJECT INCLUDES - Local Header Files
 * ================================================================ */
#include "ne.h" /* Main neqn header with type definitions */

/* ================================================================
 * MODULE-LEVEL CONSTANTS AND MACROS
 * ================================================================ */

/**
 * @def NEQN_VERSION_MAJOR
 * @brief Major version number of the neqn processor
 */
#define NEQN_VERSION_MAJOR 2

/**
 * @def NEQN_VERSION_MINOR  
 * @brief Minor version number of the neqn processor
 */
#define NEQN_VERSION_MINOR 0

/**
 * @def NEQN_VERSION_PATCH
 * @brief Patch level of the neqn processor
 */
#define NEQN_VERSION_PATCH 0

/**
 * @def NEQN_BUILD_DATE
 * @brief Compilation timestamp for version tracking
 */
#define NEQN_BUILD_DATE __DATE__ " " __TIME__

/**
 * @def NEQN_MAX_INIT_RETRIES
 * @brief Maximum number of initialization retry attempts
 */
#define NEQN_MAX_INIT_RETRIES 3

/* ================================================================
 * MODULE-LEVEL VARIABLES - Internal State Management
 * ================================================================ */

/**
 * @brief Global initialization state flag
 * @details
 * Tracks whether the neqn system has been properly initialized.
 * Used to prevent double-initialization and ensure cleanup occurs.
 * 
 * States:
 * - 0: Not initialized
 * - 1: Successfully initialized  
 * - -1: Initialization failed
 */
static int neqn_initialized = 0;

/**
 * @brief Count of active neqn instances
 * @details
 * Tracks the number of active neqn processing instances to ensure
 * proper resource management and prevent premature cleanup.
 */
static int neqn_instance_count = 0;

/**
 * @brief Global error state accumulator
 * @details
 * Maintains a running count of errors encountered during processing.
 * Used for final exit status determination and error reporting.
 */
static int neqn_error_count = 0;

/**
 * @brief Debug output level control
 * @details
 * Controls the verbosity of debug output:
 * - 0: No debug output (production mode)
 * - 1: Basic status messages
 * - 2: Detailed tracing information
 * - 3: Verbose debugging with internal state dumps
 */
static int neqn_debug_level = 0;

/**
 * @brief Tracking information for installed signal handlers
 */
typedef struct {
#ifdef SIGINT
    void (*prev_int)(int); /**< Previous SIGINT handler */
#endif
#ifdef SIGTERM
    void (*prev_term)(int); /**< Previous SIGTERM handler */
#endif
#ifdef SIGHUP
    void (*prev_hup)(int); /**< Previous SIGHUP handler */
#endif
    int installed; /**< Flag indicating handlers installed */
} neqn_signal_state_t;

/** Global signal handler state */
static neqn_signal_state_t neqn_sig_state = {0};

/** Subsystem initialization flag */
static int neqn_subsystems_initialized = 0;

/* ================================================================
 * FORWARD FUNCTION DECLARATIONS - C90 Style Prototypes
 * ================================================================ */

/* Core system functions */
static int neqn_validate_environment(void);
static int neqn_setup_signal_handlers(void);
static int neqn_initialize_subsystems(void);
static void neqn_cleanup_subsystems(void);
static void neqn_internal_signal_handler(int sig);

/* Utility and helper functions */
static void neqn_print_version_info(void);
static void neqn_print_build_info(void);
static const char *neqn_get_error_string(int error_code);

/* Debug and diagnostic functions */
static void neqn_debug_print_state(void);
static int neqn_validate_internal_state(void);

/* ================================================================
 * PUBLIC FUNCTION IMPLEMENTATIONS
 * ================================================================ */

/**
 * @brief Initialize the neqn preprocessor system
 * @details
 * Performs comprehensive system initialization including:
 * - Environment validation
 * - Signal handler setup  
 * - Subsystem initialization
 * - Global state preparation
 * 
 * This function is idempotent - calling it multiple times is safe
 * and will not cause double-initialization.
 *
 * @return NEQN_SUCCESS on successful initialization,
 *         NEQN_ERROR_INVALID on error
 *
 * @note This function must be called before using any other neqn
 *       functionality. It is thread-safe for the first call only.
 *
 * @see neqn_cleanup() for corresponding cleanup function
 *
 * @example
 * @code
 * if (neqn_init() != NEQN_INIT_SUCCESS) {
 *     fprintf(stderr, "Failed to initialize neqn system\n");
 *     exit(EXIT_FAILURE);
 * }
 * @endcode
 */
int neqn_init(void) {
    int retry_count = 0;
    int result = NEQN_ERROR_INVALID;

    /* Check if already initialized */
    if (neqn_initialized == 1) {
        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Already initialized, skipping\n");
        }
        return NEQN_SUCCESS;
    }

    /* Check if previous initialization failed */
    if (neqn_initialized == -1) {
        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Previous initialization failed\n");
        }
        return NEQN_ERROR_INVALID;
    }

    /* Attempt initialization with retry logic */
    for (retry_count = 0; retry_count < NEQN_MAX_INIT_RETRIES; retry_count++) {
        if (neqn_debug_level > 1) {
            fprintf(stderr, "neqn: Initialization attempt %d of %d\n",
                    retry_count + 1, NEQN_MAX_INIT_RETRIES);
        }

        /* Step 1: Validate runtime environment */
        if (neqn_validate_environment() != 0) {
            if (neqn_debug_level > 0) {
                fprintf(stderr, "neqn: Environment validation failed\n");
            }
            continue;
        }

        /* Step 2: Setup signal handlers for clean shutdown */
        if (neqn_setup_signal_handlers() != 0) {
            if (neqn_debug_level > 0) {
                fprintf(stderr, "neqn: Signal handler setup failed\n");
            }
            continue;
        }

        /* Step 3: Initialize all subsystems */
        if (neqn_initialize_subsystems() != 0) {
            if (neqn_debug_level > 0) {
                fprintf(stderr, "neqn: Subsystem initialization failed\n");
            }
            continue;
        }

        /* Step 4: Validate final state */
        if (neqn_validate_internal_state() != 0) {
            if (neqn_debug_level > 0) {
                fprintf(stderr, "neqn: Post-init state validation failed\n");
            }
            continue;
        }

        /* Success - break out of retry loop */
        result = NEQN_SUCCESS;
        break;
    }

    /* Update global state based on result */
    if (result == NEQN_SUCCESS) {
        neqn_initialized = 1;
        neqn_instance_count = 0;
        neqn_error_count = 0;

        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Initialization successful\n");
            neqn_print_version_info();
        }

        if (neqn_debug_level > 2) {
            neqn_print_build_info();
            neqn_debug_print_state();
        }
    } else {
        neqn_initialized = -1;

        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Initialization failed after %d attempts\n",
                    NEQN_MAX_INIT_RETRIES);
        }
    }

    return result;
}

/**
 * @brief Clean up and shutdown the neqn system
 * @details
 * Performs orderly shutdown of all neqn subsystems and releases
 * any allocated resources. This function should be called before
 * program termination to ensure clean shutdown.
 *
 * The function is safe to call multiple times and will only perform
 * cleanup once. It will also check for active instances and warn
 * if cleanup is called while processing is still active.
 *
 * @note This function should be registered with atexit() for
 *       automatic cleanup on program termination.
 *
 * @see neqn_init() for corresponding initialization function
 *
 * @example
 * @code
 * atexit(neqn_cleanup);
 * @endcode
 */
void neqn_cleanup(void) {
    /* Check if cleanup already performed */
    if (neqn_initialized == 0) {
        return; /* Nothing to clean up */
    }

    if (neqn_debug_level > 0) {
        fprintf(stderr, "neqn: Beginning system cleanup\n");
    }

    /* Warn about active instances */
    if (neqn_instance_count > 0) {
        fprintf(stderr, "neqn: Warning - %d active instances during cleanup\n",
                neqn_instance_count);
    }

    /* Report final error statistics */
    if (neqn_error_count > 0 && neqn_debug_level > 0) {
        fprintf(stderr, "neqn: Total errors encountered: %d\n",
                neqn_error_count);
    }

    /* Perform subsystem cleanup */
    neqn_cleanup_subsystems();

    /* Reset global state */
    neqn_initialized = 0;
    neqn_instance_count = 0;
    neqn_error_count = 0;

    if (neqn_debug_level > 0) {
        fprintf(stderr, "neqn: System cleanup completed\n");
    }
}

/**
 * @brief Get current neqn version information
 * @details
 * Returns a statically allocated string containing version information
 * in the format "Major.Minor.Patch". The returned string is valid for
 * the lifetime of the program and should not be modified or freed.
 *
 * @return Pointer to static version string
 *
 * @note The returned pointer is always valid and never NULL
 *
 * @example
 * @code
 * printf("neqn version: %s\n", neqn_get_version());
 * @endcode
 */
const char *neqn_get_version(void) {
    static char version_buffer[64];
    static int version_initialized = 0;

    if (!version_initialized) {
        /* Use sprintf for C90 compatibility instead of snprintf */
        sprintf(version_buffer, "%d.%d.%d",
                NEQN_VERSION_MAJOR, NEQN_VERSION_MINOR, NEQN_VERSION_PATCH);
        version_initialized = 1;
    }

    return version_buffer;
}

/**
 * @brief Set debug output level
 * @details
 * Controls the verbosity of debug output from the neqn system.
 * Higher levels include all output from lower levels.
 *
 * @param level Debug level (0-3):
 *              - 0: No debug output (production mode)
 *              - 1: Basic status messages
 *              - 2: Detailed tracing information  
 *              - 3: Verbose debugging with internal state dumps
 *
 * @note Debug output is sent to stderr to avoid interfering with
 *       normal program output
 */
void neqn_set_debug_level(int level) {
    if (level < 0) {
        level = 0;
    } else if (level > 3) {
        level = 3;
    }

    neqn_debug_level = level;

    if (level > 0) {
        fprintf(stderr, "neqn: Debug level set to %d\n", level);
    }
}

/**
 * @brief Get current debug output level
 * @details
 * Returns the current debug verbosity level setting.
 *
 * @return Current debug level (0-3)
 */
int neqn_get_debug_level(void) {
    return neqn_debug_level;
}

/**
 * @brief Register a new active neqn processing instance
 * @details
 * Increments the internal counter of active processing instances.
 * This helps track resource usage and prevents premature cleanup.
 *
 * @return New instance count, or -1 if system not initialized
 */
int neqn_register_instance(void) {
    if (neqn_initialized != 1) {
        return -1;
    }

    neqn_instance_count++;

    if (neqn_debug_level > 1) {
        fprintf(stderr, "neqn: Registered instance %d\n", neqn_instance_count);
    }

    return neqn_instance_count;
}

/**
 * @brief Unregister an active neqn processing instance  
 * @details
 * Decrements the internal counter of active processing instances.
 * Should be called when processing completes.
 *
 * @return New instance count, or -1 if system not initialized
 */
int neqn_unregister_instance(void) {
    if (neqn_initialized != 1) {
        return -1;
    }

    if (neqn_instance_count > 0) {
        neqn_instance_count--;
    }

    if (neqn_debug_level > 1) {
        fprintf(stderr, "neqn: Unregistered instance, %d remaining\n",
                neqn_instance_count);
    }

    return neqn_instance_count;
}

/**
 * @brief Report an error to the global error tracking system
 * @details
 * Increments the global error counter and optionally prints debug
 * information about the error.
 *
 * @param error_code Error code (application-specific)
 * @param message Optional error message (can be NULL)
 */
void neqn_report_error(int error_code, const char *message) {
    neqn_error_count++;

    if (neqn_debug_level > 0) {
        fprintf(stderr, "neqn: Error %d", error_code);

        if (message != NULL) {
            fprintf(stderr, ": %s", message);
        }

        fprintf(stderr, " (total errors: %d)\n", neqn_error_count);
    }
}

/**
 * @brief Get total number of errors encountered
 * @details
 * Returns the cumulative count of errors reported since initialization.
 *
 * @return Total error count
 */
int neqn_get_error_count(void) {
    return neqn_error_count;
}

/* ================================================================
 * PRIVATE FUNCTION IMPLEMENTATIONS - Internal Helper Functions
 * ================================================================ */

/**
 * @brief Validate the runtime environment
 * @details
 * Checks that the runtime environment is suitable for neqn operation.
 * This includes validating system capabilities and resource availability.
 *
 * @return 0 on success, non-zero on validation failure
 */
static int neqn_validate_environment(void) {
    /* Check basic C library functionality */
    if (sizeof(int) < 2) {
        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Integer size too small\n");
        }
        return -1;
    }

    /* Validate memory allocation */
    {
        void *test_ptr = malloc(1024);
        if (test_ptr == NULL) {
            if (neqn_debug_level > 0) {
                fprintf(stderr, "neqn: Memory allocation test failed\n");
            }
            return -1;
        }
        free(test_ptr);
    }

    /* Check file I/O capabilities */
    if (stdin == NULL || stdout == NULL || stderr == NULL) {
        if (neqn_debug_level > 0) {
            fprintf(stderr, "neqn: Standard I/O streams not available\n");
        }
        return -1;
    }

    return 0;
}

/**
 * @brief Setup signal handlers for clean shutdown
 * @details
 * Installs signal handlers to ensure clean shutdown on common
 * termination signals. This is a no-op in the current implementation
 * but provides a hook for future enhancement.
 *
 * @return 0 on success, non-zero on failure
 */
static int neqn_setup_signal_handlers(void) {
    /* Install handlers only once */
    if (neqn_sig_state.installed) {
        return 0;
    }

#ifdef SIGINT
    neqn_sig_state.prev_int = signal(SIGINT, neqn_internal_signal_handler);
    if (neqn_sig_state.prev_int == SIG_ERR) {
        return -1;
    }
#endif

#ifdef SIGTERM
    neqn_sig_state.prev_term = signal(SIGTERM, neqn_internal_signal_handler);
    if (neqn_sig_state.prev_term == SIG_ERR) {
        return -1;
    }
#endif

#ifdef SIGHUP
    neqn_sig_state.prev_hup = signal(SIGHUP, neqn_internal_signal_handler);
    if (neqn_sig_state.prev_hup == SIG_ERR) {
        return -1;
    }
#endif

    neqn_sig_state.installed = 1;
    return 0;
}

/**
 * @brief Initialize all neqn subsystems
 * @details
 * Calls initialization functions for all neqn subsystems.
 * Currently a placeholder for future subsystem additions.
 *
 * @return 0 on success, non-zero on failure
 */
static int neqn_initialize_subsystems(void) {
    /* Prevent double initialization */
    if (neqn_subsystems_initialized) {
        return 0;
    }

    /* Register global cleanup */
    if (atexit(neqn_cleanup) != 0) {
        return -1;
    }

    /* Initialize locale for wide character support */
    if (setlocale(LC_ALL, "") == NULL) {
        return -1;
    }

    neqn_subsystems_initialized = 1;
    return 0;
}

/**
 * @brief Clean up all neqn subsystems
 * @details
 * Calls cleanup functions for all neqn subsystems in reverse
 * order of initialization.
 */
static void neqn_cleanup_subsystems(void) {
    if (neqn_sig_state.installed) {
#ifdef SIGINT
        signal(SIGINT, neqn_sig_state.prev_int);
#endif
#ifdef SIGTERM
        signal(SIGTERM, neqn_sig_state.prev_term);
#endif
#ifdef SIGHUP
        signal(SIGHUP, neqn_sig_state.prev_hup);
#endif
        neqn_sig_state.installed = 0;
    }

    neqn_subsystems_initialized = 0;
}

/**
 * @brief Internal handler for termination signals
 * @param sig Signal number
 */
static void neqn_internal_signal_handler(int sig) {
    const char *name = "UNKNOWN";

#ifdef SIGINT
    if (sig == SIGINT) {
        name = "SIGINT";
    }
#endif
#ifdef SIGTERM
    if (sig == SIGTERM) {
        name = "SIGTERM";
    }
#endif
#ifdef SIGHUP
    if (sig == SIGHUP) {
        name = "SIGHUP";
    }
#endif

    if (neqn_debug_level > 0) {
        fprintf(stderr, "\nneqn: Caught signal %s (%d)\n", name, sig);
    }

    neqn_cleanup();
    exit(EXIT_FAILURE);
}

/**
 * @brief Print version information to stderr
 * @details
 * Outputs formatted version information including version number
 * and basic system information.
 */
static void neqn_print_version_info(void) {
    fprintf(stderr, "neqn version %s\n", neqn_get_version());
    fprintf(stderr, "Mathematical equation preprocessor\n");
    fprintf(stderr, "Based on original AT&T eqn (1977)\n");
}

/**
 * @brief Print build information to stderr
 * @details
 * Outputs detailed build information including compilation timestamp
 * and system configuration details.
 */
static void neqn_print_build_info(void) {
    fprintf(stderr, "Build: %s\n", NEQN_BUILD_DATE);
    fprintf(stderr, "C90 compliant implementation\n");
    fprintf(stderr, "Integer size: %lu bytes\n", (unsigned long)sizeof(int));
    fprintf(stderr, "Pointer size: %lu bytes\n", (unsigned long)sizeof(void *));
}

/**
 * @brief Get human-readable error description
 * @details
 * Converts numeric error codes to descriptive strings.
 *
 * @param error_code Numeric error code
 * @return Pointer to static error description string
 */
static const char *neqn_get_error_string(int error_code) {
    switch (error_code) {
    case 0:
        return "No error";
    case NEQN_ERROR_INVALID:
        return "Initialization failure";
    default:
        return "Unknown error";
    }
}

/**
 * @brief Print current internal state for debugging
 * @details
 * Outputs detailed information about the current state of the
 * neqn system including initialization status, instance counts,
 * and error statistics.
 */
static void neqn_debug_print_state(void) {
    fprintf(stderr, "=== NEQN Internal State ===\n");
    fprintf(stderr, "Initialized: %d\n", neqn_initialized);
    fprintf(stderr, "Instance count: %d\n", neqn_instance_count);
    fprintf(stderr, "Error count: %d\n", neqn_error_count);
    fprintf(stderr, "Debug level: %d\n", neqn_debug_level);
    fprintf(stderr, "===========================\n");
}

/**
 * @brief Validate internal state consistency
 * @details
 * Performs internal consistency checks on global state variables
 * to detect corruption or invalid states.
 *
 * @return 0 if state is valid, non-zero if inconsistencies detected
 */
static int neqn_validate_internal_state(void) {
    /* Check for impossible state combinations */
    if (neqn_initialized == 1 && neqn_instance_count < 0) {
        return -1;
    }

    if (neqn_error_count < 0) {
        return -1;
    }

    if (neqn_debug_level < 0 || neqn_debug_level > 3) {
        return -1;
    }

    return 0;
}

/* ================================================================
 * COMPATIBILITY FUNCTIONS - Legacy Support
 * ================================================================ */

/**
 * @brief Legacy initialization function name
 * @details
 * Provides backward compatibility with older code that may call
 * the original function name.
 *
 * @deprecated Use neqn_init() instead
 */
void neqn_module_init(void) {
    if (neqn_debug_level > 0) {
        fprintf(stderr, "neqn: Warning - using deprecated neqn_module_init()\n");
        fprintf(stderr, "neqn: Please update code to use neqn_init()\n");
    }

    neqn_init();
}

/* ================================================================
 * END OF FILE - ne0.c
 * ================================================================ */
