# neqn - Mathematical Equation Preprocessor

## Overview

`neqn` is a mathematical equation preprocessor designed for terminal output. It's a simplified version of the classic `eqn` preprocessor, originally developed at AT&T Bell Labs in 1977 as part of the UNIX troff text processing system.

This is a modern C90-compliant implementation that maintains backward compatibility while adding enhanced error handling, comprehensive documentation, and improved portability.

## Features

- **C90 Compliant**: Written in strict C90 for maximum portability
- **Terminal Output**: Optimized for text terminal display
- **Mathematical Notation**: Supports common mathematical expressions
- **Error Handling**: Comprehensive error reporting with line numbers
- **Memory Safe**: Careful memory management with bounds checking
- **Extensible**: Modular design for easy enhancement

## Historical Context

- Originally part of UNIX Version 7 (1979)
- Based on the `eqn` preprocessor by Brian Kernighan and Lorinda Cherry
- Simplified version for terminal output (hence "neqn" = "new eqn")
- Part of the original UNIX troff text processing system

## Building

### Requirements

- C90-compliant compiler (GCC, Clang, etc.)
- POSIX-compatible system (Linux, macOS, BSD, etc.)
- Make utility

### Quick Build

```bash
make
```

### Debug Build

```bash
make debug
```

### Install

```bash
make install
```

This installs `neqn` to `/usr/local/bin` by default. To install to a different location:

```bash
make install PREFIX=/usr
```

## Usage

### Basic Usage

```bash
neqn [OPTIONS] [FILE...]
```

If no files are specified, `neqn` reads from standard input.

### Options

- `-h, --help` - Show help message
- `-V, --version` - Show version information  
- `-v, --verbose` - Enable verbose output
- `-d, --debug [N]` - Enable debug output (level 0-3)
- `--` - End of options marker

### Examples

Process a single equation from command line:
```bash
echo 'x sup 2 + y sup 2 = r sup 2' | neqn
```

Process a file:
```bash
neqn equations.txt > formatted.txt
```

Process multiple files with verbose output:
```bash
neqn -v input1.eqn input2.eqn
```

## Mathematical Notation

`neqn` supports a simplified mathematical notation syntax:

### Basic Operations

- `x + y` - Addition
- `x - y` - Subtraction  
- `x * y` - Multiplication
- `x / y` - Division

### Superscripts and Subscripts

- `x sup 2` - x²
- `x sub i` - xᵢ
- `x sup {2+3}` - x^(2+3)

### Fractions

- `{a+b} over {c+d}` - (a+b)/(c+d)

### Square Roots

- `sqrt {x+y}` - √(x+y)

### Grouping

- `{expression}` - Group expressions
- `()` - Parentheses

## Implementation Details

### Architecture

The `neqn` system is organized into several modules:

- **neqn.c** - Main program entry point and command-line processing
- **ne0.c** - Core initialization and utility functions
- **ne_core.c** - Core functionality including parsing and output generation
- **ne.h** - Main header file with type definitions and prototypes

### Key Features

1. **Context-Based Processing**: All operations use a processing context for thread-safety and resource management

2. **Token-Based Parsing**: Input is tokenized before parsing for better error handling

3. **Expression Trees**: Mathematical expressions are parsed into tree structures

4. **Memory Management**: Careful allocation/deallocation with error checking

5. **Error Reporting**: Detailed error messages with file and line information

## Development

### Code Style

The code follows C90 standards with:

- Function prototypes for all functions
- Explicit variable declarations at block start
- No C99/C11 features (like variable-length arrays)
- Comprehensive documentation for all functions
- Consistent naming conventions

### Testing

Run basic tests:
```bash
make test
```

Run comprehensive tests:
```bash
make check
```

### Code Quality

Run static analysis:
```bash
make lint
```

Format code:
```bash
make format
```

### Debugging

Build with debug symbols:
```bash
make debug
```

Run with debug output:
```bash
./neqn -d 3 input.txt
```

## File Structure

```
neqn/
├── Makefile          # Build system
├── README.md         # This file
├── ne.h              # Main header file
├── neqn.c            # Main program
├── ne0.c             # Initialization module
├── ne_core.c         # Core functionality
└── obj/              # Build objects (created by make)
```

## Contributing

### Adding New Features

1. Add function prototypes to `ne.h`
2. Implement functions in appropriate module
3. Update documentation
4. Add tests if applicable

### Code Guidelines

- Follow C90 standards strictly
- Add comprehensive documentation
- Include error handling
- Use consistent naming conventions
- Test on multiple platforms

## License

Copyright (c) 1979 The Regents of the University of California.
Copyright (c) 2023 Modern contributors.
All rights reserved.

This software was developed by the University of California, Berkeley
and its contributors. It may be freely redistributed under the terms
of the Berkeley Software Distribution license.

## See Also

- `eqn(1)` - The original equation preprocessor
- `troff(1)` - Text formatting system
- `groff(1)` - GNU version of troff

## Authors

- **Original**: AT&T Bell Labs (1977)
- **Modern C90 port**: Various contributors

## Version History

- **2.0** - Modern C90-compliant implementation with enhanced error handling
- **1.x** - Original AT&T implementation (1977-1979)

---

For more information, see the manual page or run `neqn --help`.
