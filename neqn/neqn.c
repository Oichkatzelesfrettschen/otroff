/* C17 - no scaffold needed */
/**
 * @file neqn.c
 * @brief Main entry point for the neqn mathematical equation preprocessor
 *
 * @details
 * This is the main driver program for neqn, a mathematical equation
 * preprocessor designed for terminal output. It handles command-line
 * arguments, sets up the processing context, and coordinates the
 * overall equation processing workflow.
 *
 * Historical Context:
 * - Originally developed at AT&T Bell Labs (1977)
 * - Simplified version of eqn for terminal output
 * - Part of the original UNIX troff text processing system
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
 */

/* ================================================================
 * SYSTEM INCLUDES - C90 Standard Library Headers
 * ================================================================ */
#include <stdio.h> /* Standard I/O operations */
#include <stdlib.h> /* Memory allocation, program control */
#include <string.h> /* String manipulation functions */
#include <cctype> /* Character classification */
#include <cerrno> /* Error number definitions */
#include <signal.h> /* Signal handling */

/* ================================================================
 * PROJECT INCLUDES - Local Header Files
 * ================================================================ */
#include "ne.h" /* Main neqn header with type definitions */

/* ================================================================
 * PROGRAM CONSTANTS AND CONFIGURATION
 * ================================================================ */

/**
 * @def NEQN_PROGRAM_NAME
 * @brief Program name for error messages and usage
 */
#define NEQN_PROGRAM_NAME "neqn"

/**
 * @def NEQN_DEFAULT_OUTPUT_WIDTH
 * @brief Default output line width for terminal display
 */
#define NEQN_DEFAULT_OUTPUT_WIDTH 80

/**
 * @def NEQN_EXIT_SUCCESS
 * @brief Exit code for successful completion
 */
#define NEQN_EXIT_SUCCESS 0

/**
 * @def NEQN_EXIT_FAILURE
 * @brief Exit code for program failure
 */
#define NEQN_EXIT_FAILURE 1

/**
 * @def NEQN_EXIT_USAGE
 * @brief Exit code for usage/argument errors
 */
#define NEQN_EXIT_USAGE 2

/* ================================================================
 * GLOBAL VARIABLES - Program State
 * ================================================================ */

/**
 * @brief Current processing context (defined in ne.h as extern)
 */
neqn_context_t *neqn_current_context = NULL;

/**
 * @brief Program exit status (defined in ne.h as extern)
 */
int neqn_exit_status = NEQN_EXIT_SUCCESS;

/**
 * @brief Program name for error messages
 */
static const char *program_name = NEQN_PROGRAM_NAME;

/**
 * @brief Verbose output flag
 */
static int verbose_mode = 0;

/**
 * @brief Version display flag
 */
static int show_version = 0;

/**
 * @brief Help display flag
 */
static int show_help = 0;

/* ================================================================
 * FORWARD FUNCTION DECLARATIONS
 * ================================================================ */

static void print_usage(void);
static void print_version(void);
static void print_help(void);
static int parse_command_line(int argc, char *argv[]);
static int process_input_files(char **input_files, int file_count);
static int process_single_file(const char *filename);
static void cleanup_and_exit(int exit_code) NEQN_NORETURN;
static void signal_handler(int sig);
static int setup_signal_handlers(void);

/* ================================================================
 * MAIN PROGRAM ENTRY POINT
 * ================================================================ */

/**
 * @brief Main entry point for the neqn program
 * @details
 * Processes command-line arguments, initializes the neqn system,
 * and coordinates the processing of input files or stdin.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status code
 */
int main(int argc, char *argv[]) {
    int result = NEQN_EXIT_SUCCESS;
    char **input_files = NULL;
    int file_count = 0;

    /* Set up program name for error messages */
    if (argc > 0 && argv[0] != NULL) {
        const char *basename = strrchr(argv[0], '/');
        program_name = (basename != NULL) ? basename + 1 : argv[0];
    }

    /* Parse command-line arguments */
    file_count = parse_command_line(argc, argv);
    if (file_count < 0) {
        cleanup_and_exit(NEQN_EXIT_USAGE);
    }

    /* Handle special flags */
    if (show_version) {
        print_version();
        cleanup_and_exit(NEQN_EXIT_SUCCESS);
    }

    if (show_help) {
        print_help();
        cleanup_and_exit(NEQN_EXIT_SUCCESS);
    }

    /* Set up signal handlers for clean shutdown */
    if (setup_signal_handlers() != 0) {
        fprintf(stderr, "%s: Warning - could not set up signal handlers\n",
                program_name);
    }

    /* Initialize the neqn system */
    if (neqn_init() != NEQN_SUCCESS) {
        fprintf(stderr, "%s: Failed to initialize neqn system\n", program_name);
        cleanup_and_exit(NEQN_EXIT_FAILURE);
    }

    /* Register cleanup function */
    if (atexit(neqn_cleanup) != 0) {
        fprintf(stderr, "%s: Warning - could not register cleanup function\n",
                program_name);
    }

    /* Create processing context */
    neqn_current_context = neqn_context_create();
    if (neqn_current_context == NULL) {
        fprintf(stderr, "%s: Failed to create processing context\n", program_name);
        cleanup_and_exit(NEQN_EXIT_FAILURE);
    }

    /* Set debug level if verbose mode enabled */
    if (verbose_mode) {
        neqn_set_debug_level(1);
        neqn_current_context->debug_level = 1;
    }

    /* Extract input files from command line arguments */
    if (file_count > 0) {
        int i;
        input_files = static_cast<char **>(malloc(file_count * sizeof(char *)));
        if (input_files == NULL) {
            fprintf(stderr, "%s: Memory allocation failed\n", program_name);
            cleanup_and_exit(NEQN_EXIT_FAILURE);
        }

        /* Collect non-option arguments as input files */
        for (i = 1; i < argc && file_count > 0; i++) {
            if (argv[i][0] != '-') {
                input_files[--file_count] = argv[i];
            }
        }
        file_count = argc - 1 - file_count; /* Restore actual count */
    }

    /* Process input files or stdin */
    result = process_input_files(input_files, file_count);

    /* Clean up and exit */
    if (input_files != NULL) {
        free(input_files);
    }

    cleanup_and_exit(result);
    return result; /* Never reached, but satisfies compiler */
}

/* ================================================================
 * COMMAND-LINE PROCESSING FUNCTIONS
 * ================================================================ */

/**
 * @brief Parse command-line arguments
 * @details
 * Processes all command-line options and flags, setting global
 * variables accordingly.
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Number of input files, or -1 on error
 */
static int parse_command_line(int argc, char *argv[]) {
    int i;
    int file_count = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            /* Process options */
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose_mode = 1;
            } else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0) {
                show_version = 1;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                show_help = 1;
            } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
                verbose_mode = 1;
                if (i + 1 < argc && isdigit(argv[i + 1][0])) {
                    int debug_level = atoi(argv[++i]);
                    neqn_set_debug_level(debug_level);
                } else {
                    neqn_set_debug_level(2);
                }
            } else if (strcmp(argv[i], "--") == 0) {
                /* End of options marker */
                i++;
                break;
            } else {
                fprintf(stderr, "%s: Unknown option: %s\n", program_name, argv[i]);
                print_usage();
                return -1;
            }
        } else {
            /* Count input files */
            file_count++;
        }
    }

    /* Count remaining arguments as files */
    while (i < argc) {
        file_count++;
        i++;
    }

    return file_count;
}

/**
 * @brief Print program usage information
 */
static void print_usage(void) {
    printf("Usage: %s [OPTIONS] [FILE...]\n", program_name);
    printf("\nOptions:\n");
    printf("  -h, --help       Show this help message\n");
    printf("  -V, --version    Show version information\n");
    printf("  -v, --verbose    Enable verbose output\n");
    printf("  -d, --debug [N]  Enable debug output (level 0-3)\n");
    printf("  --               End of options\n");
    printf("\nIf no files are specified, reads from stdin.\n");
}

/**
 * @brief Print version information
 */
static void print_version(void) {
    printf("%s version %s\n", program_name, neqn_get_version());
    printf("Mathematical equation preprocessor for terminal output\n");
    printf("Based on original AT&T eqn (1977)\n");
    printf("C90 compliant implementation\n");
}

/**
 * @brief Print detailed help information
 */
static void print_help(void) {
    print_version();
    printf("\n");
    print_usage();
    printf("\nDescription:\n");
    printf("  %s is a mathematical equation preprocessor that formats\n", program_name);
    printf("  equations for display on text terminals. It reads mathematical\n");
    printf("  notation in a simplified syntax and outputs formatted text.\n");
    printf("\nExamples:\n");
    printf("  echo 'x sup 2 + y sup 2 = r sup 2' | %s\n", program_name);
    printf("  %s equations.txt > formatted.txt\n", program_name);
    printf("  %s -v input1.eqn input2.eqn\n", program_name);
}

/* ================================================================
 * FILE PROCESSING FUNCTIONS
 * ================================================================ */

/**
 * @brief Process multiple input files
 * @details
 * Iterates through the list of input files, processing each one
 * in sequence. If no files are specified, processes stdin.
 *
 * @param input_files Array of input filenames
 * @param file_count Number of input files
 * @return NEQN_EXIT_SUCCESS on success, error code on failure
 */
static int process_input_files(char **input_files, int file_count) {
    int i;
    int result = NEQN_EXIT_SUCCESS;

    if (file_count == 0) {
        /* Process stdin */
        if (verbose_mode) {
            fprintf(stderr, "%s: Processing standard input\n", program_name);
        }

        if (neqn_context_set_input(neqn_current_context, NULL) != NEQN_SUCCESS) {
            fprintf(stderr, "%s: Failed to set up standard input\n", program_name);
            return NEQN_EXIT_FAILURE;
        }

        result = process_single_file(NULL);
    } else {
        /* Process each input file */
        for (i = 0; i < file_count; i++) {
            if (verbose_mode) {
                fprintf(stderr, "%s: Processing file: %s\n",
                        program_name, input_files[i]);
            }

            if (process_single_file(input_files[i]) != NEQN_EXIT_SUCCESS) {
                result = NEQN_EXIT_FAILURE;
                /* Continue processing other files */
            }
        }
    }

    return result;
}

/**
 * @brief Process a single input file
 * @details
 * Sets up the input file, processes all lines, and handles
 * any errors that occur during processing.
 *
 * @param filename Input filename (NULL for stdin)
 * @return NEQN_EXIT_SUCCESS on success, error code on failure
 */
static int process_single_file(const char *filename) {
    char *line_buffer = NULL;
    size_t line_capacity = 0;
    int line_length;
    int line_number = 0;
    int result = NEQN_EXIT_SUCCESS;

    /* Set up input file */
    if (neqn_context_set_input(neqn_current_context, filename) != NEQN_SUCCESS) {
        fprintf(stderr, "%s: Cannot open file: %s\n",
                program_name, filename ? filename : "stdin");
        return NEQN_EXIT_FAILURE;
    }

    /* Register this processing instance */
    if (neqn_register_instance() < 0) {
        fprintf(stderr, "%s: internal error registering instance\n", program_name);
        return NEQN_EXIT_FAILURE;
    }

    /* Process each line */
    while ((line_length = neqn_read_line(neqn_current_context,
                                         &line_buffer,
                                         &line_capacity)) >= 0) {
        line_number++;
        neqn_current_context->line_number = line_number;

        /* Process the line */
        if (neqn_process_line(neqn_current_context, line_buffer) != NEQN_SUCCESS) {
            result = NEQN_EXIT_FAILURE;
            /* Continue processing for error reporting */
        }

        /* Check for EOF */
        if (line_length == 0) {
            break;
        }
    }

    /* Clean up */
    if (line_buffer != NULL) {
        free(line_buffer);
    }

    /* Report statistics if in verbose mode */
    if (verbose_mode) {
        fprintf(stderr, "%s: Processed %d lines", program_name, line_number);
        if (filename != NULL) {
            fprintf(stderr, " from %s", filename);
        }
        fprintf(stderr, "\n");

        if (neqn_current_context->error_count > 0) {
            fprintf(stderr, "%s: %d errors encountered\n",
                    program_name, neqn_current_context->error_count);
        }

        if (neqn_current_context->warning_count > 0) {
            fprintf(stderr, "%s: %d warnings issued\n",
                    program_name, neqn_current_context->warning_count);
        }
    }

    /* Unregister processing instance */
    if (neqn_unregister_instance() < 0) {
        fprintf(stderr, "%s: internal error unregistering instance\n", program_name);
    }

    return result;
}

/* ================================================================
 * CLEANUP AND EXIT FUNCTIONS
 * ================================================================ */

/**
 * @brief Clean up resources and exit with specified code
 * @details
 * Performs comprehensive cleanup of all allocated resources
 * and exits the program with the specified exit code.
 *
 * @param exit_code Exit status code
 */
static void cleanup_and_exit(int exit_code) {
    /* Clean up processing context */
    if (neqn_current_context != NULL) {
        neqn_context_destroy(neqn_current_context);
        neqn_current_context = NULL;
    }

    /* Report final statistics if in verbose mode */
    if (verbose_mode && neqn_get_error_count() > 0) {
        fprintf(stderr, "%s: Total errors: %d\n",
                program_name, neqn_get_error_count());
    }

    /* Set global exit status */
    neqn_exit_status = exit_code;

    /* Exit the program */
    exit(exit_code);
}

/* ================================================================
 * SIGNAL HANDLING FUNCTIONS
 * ================================================================ */

/**
 * @brief Signal handler for clean shutdown
 * @details
 * Handles termination signals to ensure clean shutdown
 * and proper resource cleanup.
 *
 * @param sig Signal number
 */
static void signal_handler(int sig) {
    const char *signal_name = "Unknown";

    /* Determine signal name for logging */
    switch (sig) {
    case SIGINT:
        signal_name = "SIGINT";
        break;
    case SIGTERM:
        signal_name = "SIGTERM";
        break;
#ifdef SIGHUP
    case SIGHUP:
        signal_name = "SIGHUP";
        break;
#endif
    default:
        break;
    }

    /* Log signal reception if in verbose mode */
    if (verbose_mode) {
        fprintf(stderr, "\n%s: Received signal %s (%d), shutting down...\n",
                program_name, signal_name, sig);
    }

    /* Perform clean shutdown */
    cleanup_and_exit(NEQN_EXIT_FAILURE);
}

/**
 * @brief Set up signal handlers for clean shutdown
 * @details
 * Installs signal handlers for common termination signals
 * to ensure clean shutdown on external termination.
 *
 * @return 0 on success, -1 on failure
 */
static int setup_signal_handlers(void) {
#ifdef SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        return -1;
    }
#endif

#ifdef SIGTERM
    if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        return -1;
    }
#endif

#ifdef SIGHUP
    if (signal(SIGHUP, signal_handler) == SIG_ERR) {
        return -1;
    }
#endif

    return 0;
}

/* ================================================================
 * END OF FILE - neqn.c
 * ================================================================ */
