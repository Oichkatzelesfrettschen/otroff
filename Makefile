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

CFLAGS ?= -std=c90 -Wall -O2 -march=$(MARCH)
LDFLAGS ?=

# Collect source files across the project.  Only the modern C
# replacements located under `roff/` are built.
# Source files for each project directory
# Automatically collect all modern C implementations and ignore the
# historical PDP-11 assembly sources (*.s).
ROFF_SRC := $(sort $(wildcard roff/*.c))
# Legacy sources under `croff`, `tbl`, and `neqn` are currently
# excluded from the default build as they require significant
# modernisation work.
CROFF_SRC :=
TBL_SRC   :=
NEQN_SRC  :=

# Device driver sources for troff
CROFF_TERMS :=

OBJDIR := build

# Map source files to objects inside $(OBJDIR)
ROFF_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(ROFF_SRC))
CROFF_OBJ :=
TBL_OBJ   :=
NEQN_OBJ  :=
CROFF_TERM_OBJ :=

ALL_OBJ := $(ROFF_OBJ)


# SSE accelerated routines were originally implemented in assembly.
# They are now provided as portable C sources and will be built
# automatically as part of $(ROFF_SRC).

# Default target builds everything
all: $(ALL_OBJ)

# Build individual components
croff: $(CROFF_OBJ) $(CROFF_TERM_OBJ)
tbl:   $(TBL_OBJ)
neqn:  $(NEQN_OBJ)
roff:  $(ROFF_OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean croff tbl neqn roff test

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
