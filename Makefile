# Use clang as the compiler. The built-in 'cc' is typically GCC which
# yields slightly different diagnostics. Force clang unless the user
# explicitly overrides it on the command line.
CC := clang
CPU ?= native

# Map CPU values to 64-bit -march options
ifeq ($(CPU),x86-64)
    MARCH := x86-64
else ifeq ($(CPU),haswell)
    MARCH := haswell
else
    MARCH := $(CPU)
endif

CFLAGS ?= -std=c90 -Wall -O2 -march=$(MARCH) -fopenmp=libgomp -Isrc/os
LDFLAGS ?= -fopenmp=libgomp

# Collect source files across the project.  Only the modern C
# replacements located under `roff/` are built.
# Source files for each project directory
# Automatically collect all modern C implementations and ignore the
# historical PDP-11 assembly sources (*.s).
ROFF_SRC := $(sort $(wildcard roff/*.c))
# Operating-system abstraction layer sources
OS_SRC   := src/os/os_unix.c
STUBS_SRC := src/stubs.c
# Legacy sources under `croff`, `tbl`, and `neqn` were historically
# excluded from the default build.  They can now be compiled on
# demand via dedicated make targets.  Collect all C sources within
# each directory so the objects can be produced automatically.
CROFF_SRC := $(sort $(wildcard croff/*.c))
TBL_SRC   := $(sort $(wildcard tbl/*.c))
NEQN_SRC  := $(sort $(wildcard neqn/*.c))

# Device driver sources for troff
CROFF_TERMS :=

OBJDIR := build

# Map source files to objects inside $(OBJDIR)
ROFF_OBJ        := $(patsubst %.c,$(OBJDIR)/%.o,$(ROFF_SRC))
CROFF_OBJ       := $(patsubst %.c,$(OBJDIR)/%.o,$(CROFF_SRC))
TBL_OBJ         := $(patsubst %.c,$(OBJDIR)/%.o,$(TBL_SRC))
NEQN_OBJ        := $(patsubst %.c,$(OBJDIR)/%.o,$(NEQN_SRC))
OS_OBJ          := $(patsubst %.c,$(OBJDIR)/%.o,$(OS_SRC))
STUBS_OBJ       := $(patsubst %.c,$(OBJDIR)/%.o,$(STUBS_SRC))
# Device driver objects are chosen via the CROFF_TERMS variable.
CROFF_TERM_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(CROFF_TERMS))

# Object lists used for linking and compilation
TROFF_OBJ := $(ROFF_OBJ) $(OS_OBJ) $(STUBS_OBJ)
# Union of all object files produced by this Makefile
ALL_OBJ   := $(TROFF_OBJ) $(CROFF_OBJ) $(TBL_OBJ) $(NEQN_OBJ) $(CROFF_TERM_OBJ)


# SSE accelerated routines were originally implemented in assembly.
# They are now provided as portable C sources and will be built
# automatically as part of $(ROFF_SRC).

# Default target builds everything
all: $(OBJDIR)/troff

$(OBJDIR)/troff: $(TROFF_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Build individual components
croff: $(CROFF_OBJ) $(CROFF_TERM_OBJ)
tbl:   $(TBL_OBJ)
neqn:  $(NEQN_OBJ)
roff:  $(ROFF_OBJ)

# Compile any object residing in $(ALL_OBJ).  The static pattern rule
# ensures sources from all subdirectories share consistent compiler
# flags and output locations under $(OBJDIR).
	$(ALL_OBJ): $(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean croff tbl neqn roff test format

# Build using CMake in a separate directory
.PHONY: cmake
cmake:
	cmake -S . -B build-cmake
	cmake --build build-cmake

# Build using Meson in a separate directory
.PHONY: meson
meson:
	meson setup build-meson --reconfigure || meson setup build-meson
	ninja -C build-meson

# Run the test-suite using pytest
test:
	pytest -q

# Format all C and header files using clang-format
format:
	find . \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 clang-format -i
