# NEQN PROJECT STRUCTURE

This document provides a comprehensive overview of the neqn mathematical equation preprocessor project structure, implementation details, and usage guidelines.

## Project Overview

**neqn** (n-equation) is a mathematical equation preprocessor designed to format mathematical notation for terminal and text-based output. It provides a simplified alternative to the traditional `eqn` preprocessor, focusing on readability and ease of use in modern text processing pipelines.

## File Structure

```
neqn/
├── src/
│   ├── neqn.c           # Main program entry point
│   ├── ne0.c            # Core parsing and processing engine  
│   ├── ne_core.c        # AST management and utilities
│   ├── ne_symbols.c     # Symbol table and mathematical notation
│   └── ne.h             # Main header file with all declarations
├── examples/
│   ├── basic_equations.txt    # Simple mathematical expressions
│   └── advanced_equations.txt # Complex mathematical notation
├── tests/
│   └── test_neqn.c      # Unit tests and validation
├── Makefile             # Build configuration
├── test_runner.sh       # Automated test suite
├── README.md            # User documentation
└── PROJECT_STRUCTURE.md # This file
```

## Core Components

### 1. Main Program (`neqn.c`)

The main entry point handles:
- Command-line argument parsing
- Input/output stream management
- Context initialization and cleanup
- Signal handling for graceful termination

**Key Functions:**
- `main()` - Program entry point
- `neqn_process_stream()` - Process input line by line
- `signal_handler()` - Handle interruption signals

### 2. Parser Engine (`ne0.c`)

The core parsing engine responsible for:
- Lexical analysis and tokenization
- Grammar parsing and validation
- Error reporting and recovery
- Integration with symbol table

**Key Functions:**
- `neqn_parse_line()` - Parse a single input line
- `neqn_tokenize()` - Break input into tokens
- `neqn_parse_expression()` - Build syntax trees
- `neqn_error()` / `neqn_warning()` - Error reporting

### 3. AST Management (`ne_core.c`)

Abstract Syntax Tree operations including:
- Node creation and management
- Tree traversal and manipulation
- Memory management and cleanup
- Context state management

**Key Functions:**
- `neqn_context_create()` / `neqn_context_destroy()`
- `neqn_node_create()` / `neqn_node_destroy()`
- `neqn_tree_format()` - Convert AST to output
- `neqn_hash_string()` - String hashing for symbol table

### 4. Symbol Management (`ne_symbols.c`)

Mathematical symbol handling including:
- Built-in mathematical symbols (Greek letters, operators)
- User-defined symbol definitions
- Symbol lookup and substitution
- Mathematical formatting functions

**Key Functions:**
- `neqn_init_builtin_symbols()` - Initialize predefined symbols
- `neqn_symbol_lookup_enhanced()` - Symbol resolution
- `neqn_superscript()` / `neqn_subscript()` - Formatting functions
- `neqn_fraction()` / `neqn_sqrt()` - Mathematical constructs

## Data Structures

### Context Structure (`neqn_context_t`)
```c
typedef struct neqn_context {
    neqn_symbol_t *symbols[NEQN_HASH_SIZE];  // Symbol hash table
    int line_number;                          // Current line number
    int error_count;                          // Error tracking
    int warning_count;                        // Warning tracking
    int flags;                               // Processing flags
} neqn_context_t;
```

### AST Node Structure (`neqn_node_t`)
```c
typedef struct neqn_node {
    neqn_node_type_t type;    // Node type (identifier, operator, etc.)
    char *content;            // Node content/value
    struct neqn_node *left;   // Left child
    struct neqn_node *right;  // Right child
    struct neqn_node *next;   // Next sibling
} neqn_node_t;
```

### Symbol Structure (`neqn_symbol_t`)
```c
typedef struct neqn_symbol {
    char *name;                    // Symbol name
    char *value;                   // Symbol value/definition
    neqn_node_t *tree;            // Associated AST
    int line_defined;             // Line where defined
    int is_builtin;               // Built-in flag
    struct neqn_symbol *next;     // Hash table chaining
} neqn_symbol_t;
```

## Mathematical Notation Support

### Basic Operations
- **Arithmetic**: `+`, `-`, `*`, `/`
- **Comparison**: `=`, `!=`, `<`, `>`, `<=`, `>=`
- **Superscript**: `x sup 2` → x²
- **Subscript**: `x sub i` → xᵢ

### Advanced Constructs
- **Fractions**: `{a+b} over {c+d}` → (a+b)/(c+d)
- **Square Root**: `sqrt{expression}` → √(expression)
- **Summation**: `sum from i=1 to n` → Σ notation
- **Integration**: `int from a to b` → ∫ notation

### Greek Letters and Symbols
- **Letters**: `alpha`, `beta`, `gamma`, `delta`, `pi`, etc.
- **Operators**: `+-`, `times`, `div`, `approx`, etc.
- **Set Theory**: `subset`, `union`, `inter`, `in`, etc.
- **Logic**: `and`, `or`, `not`, `=>`, `<=>`, etc.

## Build System

### Makefile Targets
- `make all` - Build main program
- `make test` - Build and run unit tests
- `make check` - Run comprehensive tests
- `make clean` - Clean build artifacts
- `make install` - Install to system (requires privileges)

### Compiler Configuration
- **Standard**: C90/C99 compatible
- **Warnings**: Comprehensive warning flags enabled
- **Optimization**: -O2 for release builds
- **Debug**: -g flag for debugging builds

## Testing Framework

### Unit Tests (`test_neqn.c`)
- Core functionality validation
- Error handling verification
- Memory leak detection
- Edge case testing

### Integration Tests (`test_runner.sh`)
- End-to-end functionality
- Example file processing
- Performance benchmarking
- Error handling validation

### Example Files
- **Basic**: Simple mathematical expressions
- **Advanced**: Complex mathematical notation
- **Edge Cases**: Boundary conditions and error cases

## Error Handling

### Error Categories
1. **Syntax Errors**: Invalid mathematical notation
2. **Memory Errors**: Allocation failures
3. **IO Errors**: File/stream access issues
4. **Logic Errors**: Invalid mathematical constructs

### Error Reporting
- Line number tracking
- Descriptive error messages
- Warning vs. error classification
- Graceful degradation where possible

## Memory Management

### Allocation Strategy
- **Small objects**: Direct malloc/free
- **Large structures**: Reference counting
- **Strings**: Copy-on-write where beneficial
- **Trees**: Recursive cleanup

### Leak Prevention
- Consistent cleanup functions
- Error path cleanup
- Signal handler cleanup
- Unit test validation

## Performance Considerations

### Optimization Techniques
- **Hash tables**: O(1) symbol lookup
- **String interning**: Reduce memory overhead
- **Lazy evaluation**: Defer expensive operations
- **Caching**: Avoid redundant computations

### Scalability
- **Large inputs**: Streaming processing
- **Memory usage**: Bounded by hash table size
- **Processing speed**: Linear in input size
- **Symbol tables**: Efficient for large symbol sets

## Extension Points

### Adding New Mathematical Constructs
1. Define new node types in `ne.h`
2. Add parsing logic in `ne0.c`
3. Implement formatting in `ne_symbols.c`
4. Add tests for new functionality

### Custom Symbol Sets
1. Extend `builtin_symbols` array
2. Add formatting functions as needed
3. Update documentation
4. Validate with test cases

## Compatibility

### Platform Support
- **Unix/Linux**: Primary target platform
- **macOS**: Fully supported
- **Windows**: Compatible with MSYS2/Cygwin
- **Embedded**: Minimal resource requirements

### Standard Compliance
- **C Standard**: C90 minimum, C99 features where beneficial
- **POSIX**: Basic POSIX.1 compliance
- **Unicode**: UTF-8 input/output support
- **Locale**: Respects system locale settings

## Future Enhancements

### Planned Features
1. **LaTeX output**: Generate LaTeX-formatted equations
2. **MathML support**: XML mathematical markup
3. **Interactive mode**: REPL for equation editing
4. **Macro system**: User-defined mathematical constructs

### Performance Improvements
1. **Parser optimization**: Reduce memory allocations
2. **Parallel processing**: Multi-threaded symbol resolution
3. **Caching**: Intelligent result caching
4. **Streaming**: Better large-file handling

## Contributing

### Code Style
- **Indentation**: 4 spaces, no tabs
- **Naming**: `snake_case` for functions, `UPPER_CASE` for constants
- **Comments**: Doxygen-style documentation
- **Line length**: 80 characters maximum

### Testing Requirements
- All new features must include unit tests
- Integration tests for user-facing changes
- Performance tests for optimization changes
- Documentation updates for all changes

### Submission Process
1. Fork the repository
2. Create feature branch
3. Implement changes with tests
4. Update documentation
5. Submit pull request

## License and Copyright

This project is part of the troff/groff ecosystem and follows the same licensing terms. See the main project documentation for specific license information.

---

For more information, see README.md or contact the maintainers.
