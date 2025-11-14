# File Extension Rationalization Plan

## THE TRUTH: What This Codebase Actually Is

### Current Problem
- **95+ files** with `.cpp` extension
- **~85%** of those files are **PURE C** code with wrong extension
- **Comments say**: "Portable C90", "C99 compliant", "C standard"
- **Reality**: Compiled as C++ despite being C

### The Lie
```cpp
// File: roff/roff5.cpp
/**
 * @file roff5.c   <-- Says ".c" in comment!
 * @brief ROFF hyphenation engine
 *
 * This has been converted to portable C90...
 */

#include <cstdio>     // C++ header
namespace otroff {    // C++ namespace
// ... pure C code ...
}
```

This is **architectural dishonesty**. We're lying to ourselves and the compiler.

---

## THE PLAN: Three-Phase Rationalization

### PHASE 1: IDENTIFY (Analysis Complete)

**Category A: Already C - Wrong Extension**
These files **claim** to be C and **contain** only C code:

| File | Claim | Verdict |
|------|-------|---------|
| `roff/roff3.cpp` | "Portable C90 implementation" | Rename to `.c` |
| `roff/roff4.cpp` | "Portable C90 implementation" | Rename to `.c` |
| `roff/roff5.cpp` | "Portable C90 implementation" | Rename to `.c` |
| `roff/roff7.cpp` | Data tables | Rename to `.c` |
| `roff/roff8.cpp` | "Portable C90" | Rename to `.c` |
| `croff/n1.cpp` | "Main module for troff (C90)" | Rename to `.c` |
| `croff/n2.cpp` | "C90" | Rename to `.c` |
| `croff/n3.cpp` | "C90 compliant implementation" | Rename to `.c` |
| `croff/n4.cpp` | "C90" | Rename to `.c` |
| `croff/n7.cpp` | "C90 compliance" | Rename to `.c` |
| `croff/n8.cpp` | "Enhanced C90 compliant version" | Rename to `.c` |
| `croff/hytab.cpp` | "C90 portable" | Rename to `.c` |
| `neqn/ne0.cpp` | "C90 standards" | Rename to `.c` |
| `neqn/ne1.cpp` | C90 claim | Rename to `.c` |
| `neqn/ne2.cpp` | C90 claim | Rename to `.c` |
| `neqn/ne3.cpp` | C90 claim | Rename to `.c` |
| `neqn/ne4.cpp` | C90 claim | Rename to `.c` |
| `neqn/ne5.cpp` | C90 claim | Rename to `.c` |
| `neqn/neqn.cpp` | "C90 standards" | Rename to `.c` |
| `tbl/t0.cpp` through `tbl/t9.cpp` | Data/tables | Rename to `.c` |
| Plus ~30 more... | | |

**Category B: Actually C++ - Keep .cpp**
These files **use** C++ features meaningfully:

| File | C++ Features | Verdict |
|------|--------------|---------|
| `roff/roff1.cpp` | Classes, std::, exceptions, modern C++17 | **Keep C++** ✅ |
| `roff/roff2.cpp` | Namespaces, std::optional, std::string_view | **Keep C++** ✅ |
| `roff/roff_context.cpp` | Classes, RAII | **Keep C++** ✅ |
| `croff/troff_processor.cpp` | Classes | **Keep C++** ✅ |
| Tab driver files (`tab*.cpp`) | std:: heavy usage | **Keep C++** ✅ |

**Category C: Test Files**
- All `test_*.cpp` files → Keep as `.cpp` (use GoogleTest)

---

### PHASE 2: RENAME (Mechanical Transformation)

**Step 1: Create Migration Script**
```bash
#!/bin/bash
# migrate_c_files.sh

# Rename C files to .c extension
mv roff/roff3.cpp roff/roff3.c
mv roff/roff4.cpp roff/roff4.c
mv roff/roff5.cpp roff/roff5.c
mv roff/roff7.cpp roff/roff7.c
mv roff/roff8.cpp roff/roff8.c

# Continue for all Category A files...
```

**Step 2: Fix Headers**
```bash
#!/bin/bash
# fix_c_headers.sh

for file in $(find . -name "*.c"); do
    # C++ headers → C headers
    sed -i 's/#include <cstdio>/#include <stdio.h>/g' "$file"
    sed -i 's/#include <cstdlib>/#include <stdlib.h>/g' "$file"
    sed -i 's/#include <cstring>/#include <string.h>/g' "$file"
    sed -i 's/#include <cctype>/#include <ctype.h>/g' "$file"
    sed -i 's/#include <cmath>/#include <math.h>/g' "$file"

    # Remove C++ casts
    sed -i 's/static_cast<unsigned char>/(/g' "$file"

    # Remove namespaces
    sed -i '/^namespace otroff/d' "$file"
    sed -i '/^namespace roff_legacy/d' "$file"
    sed -i '/^} \/\/ namespace/d' "$file"
done
```

**Step 3: Update CMakeLists.txt**
```cmake
# BEFORE (wrong)
add_library(roff_core_obj OBJECT
    roff3.cpp  # C code with .cpp extension!
    roff4.cpp
    roff5.cpp
)

# AFTER (correct)
add_library(roff_core STATIC
    roff3.c    # C code with .c extension
    roff4.c
    roff5.c
)
set_target_properties(roff_core PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
)
```

---

### PHASE 3: BUILD & TEST

**Step 1: Build as C17**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

**Expected Errors**:
- Missing semicolons (C is stricter than C++)
- Variable declarations in wrong places (C requires declarations at block start)
- Implicit function declarations
- Type mismatches

**Step 2: Fix Compilation Errors**
- Add missing `#include` statements
- Fix variable declarations
- Add explicit casts where needed
- Fix any C++-isms that snuck in

**Step 3: Verify Functionality**
```bash
# Run existing tests
cd build
ctest

# Regression test - compare output with original
./roff test_input.txt > new_output.txt
diff new_output.txt golden_output.txt
```

---

### PHASE 4: MODERNIZE (After C17 Works)

**Only AFTER** C code compiles and tests pass:

**4a. Modernize C Code (C17 Features)**
```c
// Use modern C17 features
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Type-safe bool
bool is_enabled = true;  // Not int

// Fixed-width types
uint32_t flags = 0;      // Not unsigned int

// Static assertions
_Static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
```

**4b. Modernize C++ Code (C++20 Features)**
```cpp
// roff1.cpp - KEEP as modern C++

// Use C++20 features
#include <span>
#include <expected>  // or tl::expected
#include <ranges>

class RoffEngine {
    // Modern RAII
    std::unique_ptr<Context> ctx_;

    // Modern error handling
    std::expected<void, std::string> process(std::span<const char> input);

    // Ranges
    auto filter_lines(std::ranges::range auto&& lines);
};
```

---

## BENEFITS OF THIS APPROACH

### 1. **Honesty in Architecture**
- `.c` files are C
- `.cpp` files are C++
- No lying in comments

### 2. **Compiler Optimization**
- C compiler optimizes C code better
- C++ compiler optimizes C++ code better
- No mixed-mode confusion

### 3. **Correct Tooling**
- Static analyzers work correctly
- IDEs provide correct completions
- Build systems make correct decisions

### 4. **Maintainability**
- Clear what language each file uses
- No surprise C++ features in "C" files
- Easy to onboard new developers

### 5. **Portability**
- Pure C can be called from any language
- Stable C ABI
- Embedded systems can use C core

---

## TIMELINE

| Phase | Tasks | Duration | Blocker Resolution |
|-------|-------|----------|-------------------|
| **Phase 1** | Analysis | ✅ DONE | N/A |
| **Phase 2** | Rename + Fix Headers | 2 hours | None |
| **Phase 3** | Build + Fix Errors | 1-2 days | Compilation errors |
| **Phase 4** | Modernize | 1 week | Phase 3 complete |

---

## RISKS & MITIGATION

### Risk 1: "It won't compile!"
**Mitigation**: Start with one file (roff5.c), fix it, then proceed

### Risk 2: "Tests will fail!"
**Mitigation**: Have golden files for output comparison

### Risk 3: "Too much work!"
**Mitigation**: Automate with scripts, do incrementally

---

## DECISION: Start with roff5.cpp POC

**Target**: `roff/roff5.cpp` → `roff/roff5.c`

**Why**:
1. Well-documented ("Portable C90")
2. Self-contained algorithm
3. Only 833 lines
4. Clear inputs/outputs
5. Template for other files

**Success Criteria**:
1. ✅ File renamed to .c
2. ✅ Compiles as C17
3. ✅ Tests pass
4. ✅ Output matches original
5. ✅ Document lessons learned

**Next**: If POC succeeds, apply to remaining ~80 files

---

## CONCLUSION

**Current State**: Architectural lie - C code pretending to be C++

**Target State**: Honest architecture - C is C, C++ is C++

**Approach**: Incremental, test-driven, automated

**Timeline**: 2-3 weeks for full codebase

**Value**: Correct, maintainable, modern codebase on solid foundation
