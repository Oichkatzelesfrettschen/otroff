# N8.C Hyphenation Module Documentation

## Overview

The `n8.c` module implements a sophisticated hyphenation engine for the `troff` typesetting system. It provides multiple algorithms for determining appropriate word break points for line justification and formatting.

## Features

### Core Hyphenation Algorithms

1. **Exception Word List Processing**
   - Maintains a buffer of words with predetermined hyphenation points
   - Allows override of algorithmic hyphenation for special cases
   - Supports words ending in 's' with special handling

2. **Suffix-Based Hyphenation**
   - Uses suffix tables to identify common word endings
   - Effective for derived words with standard suffixes
   - Includes vowel validation for hyphenation points

3. **Bi-gram-Based Analysis**
   - Analyzes two-character patterns for hyphenation likelihood
   - Uses frequency tables to calculate "goodness" values
   - Segments words from vowel to vowel for processing

### Character Classification

- **Alphabetic Detection**: Recognizes A-Z and a-z with `troff` formatting support
- **Vowel Recognition**: Identifies a, e, i, o, u, y (y treated as vowel)
- **Punctuation Detection**: Everything non-alphabetic and non-null
- **Case Conversion**: Reliable uppercase to lowercase mapping

## Architecture

### Data Structures

```c
/* Exception word buffer */
char hbuf[NHEX];              /* Storage for exception words */
char *nexth;                  /* Next available position */

/* Word processing pointers */
int *wdstart, *wdend;         /* Current word boundaries */
int *hyend;                   /* End of hyphenation region */
int *hyptr[];                 /* Array of hyphenation points */
int **hyp;                    /* Current hyphenation pointer */

/* Configuration */
int thresh;                   /* Bi-gram goodness threshold */
int hyoff;                    /* Hyphenation offset */
```

### Algorithm Flow

1. **Word Boundary Detection**
   - Skip initial punctuation
   - Identify alphabetic characters
   - Set word start/end pointers
   - Validate minimum word length (5 characters)

2. **Multi-Stage Hyphenation**
   - Try exception word list first
   - Fall back to suffix-based analysis
   - Use bi-gram analysis as final resort
   - Sort and validate results

3. **Result Processing**
   - Sort hyphenation points
   - Remove duplicates
   - Validate vowel requirements

## Function Reference

### Core Functions

#### `void hyphenateWord(int *wp)`

Main entry point for word hyphenation.

- **Parameters**: `wp` - Pointer to word to be hyphenated
- **Returns**: None (results in global hyphenation arrays)
- **Algorithm**: Multi-stage approach with sorting

#### `int exword(void)`

Check exception word list for current word.

- **Returns**: 1 if word found in exception list, 0 otherwise
- **Features**: Handles 's' suffix variations

#### `int suffix(void)`

Perform suffix-based hyphenation analysis.

- **Returns**: 1 if hyphenation points found, 0 otherwise
### Character Classification Functions

#### `int alph(int i)`

Test if character is alphabetic.

- **Parameters**: `i` - Character (may include formatting bits)
- **Returns**: 1 if alphabetic, 0 otherwise
- **Features**: Handles `troff` formatting masks

#### `int vowel(int i)`

Test if character is a vowel.

- **Parameters**: `i` - Character to test
- **Returns**: 1 if vowel (a,e,i,o,u,y), 0 otherwise

#### `int maplow(int i)`

Convert character to lowercase.

- **Parameters**: `i` - Character to convert
- **Returns**: Lowercase version of character

#### `int punct(int i)`
#### `char *getsuf(char *x)`

Extract suffix from suffix table.

- **Parameters**: `x` - Pointer to suffix table entry
- **Returns**: Pointer to extracted suffix string

#### `int dilook(int a, int b, char t[26][13])`

Look up bi-gram value in table.

- **Parameters**: `a`, `b` - Character pair; `t` - Lookup table
- **Returns**: Bi-gram value for character pair

#### `int chkvow(int *w)`

Check if word has vowel before position.
### Hyphenation Threshold

The bi-gram threshold (`thresh`) controls hyphenation sensitivity:

- **Default**: 160
- **Higher values**: More conservative hyphenation
- **Lower values**: More aggressive hyphenation
- **Command**: `.ht [value]` in `troff`

### Exception Words

Add words to exception list with `.hw` command:
Look up digram value in table.
- **Parameters**: `a`, `b` - Character pair; `t` - Lookup table
- **Returns**: Digram value for character pair
## Integration with Troff

### Command Interface

- **`.ht [threshold]`**: Set hyphenation threshold
- **`.hw word1 word2 ...`**: Add exception words

### Data Dependencies

- **`tdef.h`**: Core `troff` definitions and constants
- **External variables**: Word processing state from main `troff` engine
- **Suffix tables**: Compressed suffix data for analysis
- **Higher values**: More conservative hyphenation
- **Lower values**: More aggressive hyphenation
- **Command**: `.ht [value]` in troff

### Exception Words

Add words to exception list with `.hw` command:
```troff
.hw hy-phen-a-tion al-go-rithm
```

## Integration with Troff

### Command Interface

- **`.ht [threshold]`**: Set hyphenation threshold
- **`.hw word1 word2 ...`**: Add exception words

### Data Dependencies

- **tdef.h**: Core troff definitions and constants
- **Bi-gram analysis**: O(w) where w = word length
- **Overall**: Linear in word length for typical cases

### Optimization Features

- Early termination when exception found
- Vowel-based segmentation reduces bi-gram calculations
- Compressed suffix tables minimize memory usage
- **Hyphenation arrays**: `NHYP` pointers maximum
- **Static buffers**: Limited temporary storage for suffix processing

### Safety Features

- Bounds checking on all buffer operations
- Null pointer validation
- Overflow protection with goto error handling

## Performance Characteristics

### Algorithmic Complexity

- **Exception lookup**: O(n) where n = exception buffer size
- **Suffix analysis**: O(m) where m = suffix table size
- **Digram analysis**: O(w) where w = word length
- **Overall**: Linear in word length for typical cases

### Optimization Features

### Unit Tests

The module includes comprehensive unit tests in `test_n8.c`:

- Character classification validation
- Word boundary detection
- Exception word handling
- Helper function verification
- Integration testing

### Build System

- **Makefile.n8**: Complete build and test automation
- **Static analysis**: `cppcheck` integration
- **Memory checking**: `valgrind` support
- **Code formatting**: `clang-format` integration

### Validation Script

Run `./validate_n8.sh` for comprehensive validation:

- C90 compliance checking
- Static analysis
- Test execution
- Memory leak detection
- Performance benchmarking

The module includes comprehensive unit tests in `test_n8.c`:
- Character classification validation
- Word boundary detection
- Exception word handling
- Helper function verification
- Integration testing

### Build System

- **Makefile.n8**: Complete build and test automation
- **Static analysis**: cppcheck integration
- **Memory checking**: valgrind support
- **Code formatting**: clang-format integration

### Validation Script

Run `./validate_n8.sh` for comprehensive validation:
- C90 compliance checking
- Static analysis
- Test execution
- Memory leak detection
- Performance benchmarking

## Error Handling

### Defensive Programming

- Input parameter validation
- Buffer overflow protection
- Graceful degradation on errors
- No undefined behavior

### Error Recovery

- Failed hyphenation falls back to no hyphenation
- Invalid inputs return safely
- Memory errors are contained

## Usage Examples

### Basic Hyphenation

```c
/* Setup word for hyphenation */
### Compatibility Considerations

All enhancements must maintain:

- C90 compatibility
- Existing API compatibility
- Memory footprint constraints
- Performance characteristics

```c
### Technical Documentation

- [Troff User's Manual](http://troff.org/)
- [ANSI C90 Standard](https://www.iso.org/standard/17782.html)
- [Hyphenation Algorithms](https://tug.org/docs/liang/)

### Related Files

- `tdef.h`: Core `troff` definitions
- `n8.c`: Main hyphenation implementation
- `test_n8.c`: Unit test suite
- `Makefile.n8`: Build system
- `validate_n8.sh`: Validation script
3. **Pattern Caching**: Cache frequently used patterns
4. **Parallel Processing**: Optimize for multi-core systems

### Compatibility Considerations

All enhancements must maintain:
- C90 compatibility
- Existing API compatibility
- Memory footprint constraints
- Performance characteristics

## References

### Technical Documentation

- [Troff User's Manual](http://troff.org/)
- [ANSI C90 Standard](https://www.iso.org/standard/17782.html)
- [Hyphenation Algorithms](https://tug.org/docs/liang/)

### Related Files

- `tdef.h`: Core troff definitions
- `n8.c`: Main hyphenation implementation
- `test_n8.c`: Unit test suite
- `Makefile.n8`: Build system
- `validate_n8.sh`: Validation script

---

**Last Updated**: 2024
**Version**: Enhanced C90 compliant version
**Maintainer**: Enhanced for modern build systems while preserving historical compatibility
