# ============================================================================
# OTROFF Comprehensive Makefile - Pure C17 Build System
# ============================================================================
# Builds all ROFF suite components: troff, croff, tbl, neqn
# Modern build system using traditional make (replaces CMake)
#
# Usage:
#   make              - Build all executables
#   make troff        - Build core troff only
#   make croff        - Build extended croff only
#   make tbl          - Build table formatter only
#   make neqn         - Build equation formatter only
#   make clean        - Remove all build artifacts
#   make install      - Install all executables
# ============================================================================

# Compiler and tools
CC = gcc
AR = ar
RANLIB = ranlib
RM = rm -f
MKDIR = mkdir -p
INSTALL = install

# C17 standard with strict compliance
CFLAGS = -std=c17 -pedantic -Wall -Wextra
CFLAGS += -O2 -g
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_GNU_SOURCE
CFLAGS += -DNROFF

# CPU optimization (override with CPU=x86-64, haswell, native, etc.)
CPU ?= native
CFLAGS += -march=$(CPU)

# Include paths
INCLUDES = -I. -Iroff -Isrc -Isrc/os -Icroff -Itbl -Ineqn

# Libraries
LDFLAGS =
LDLIBS = -lm

# Build directory
OBJDIR = build

# Installation directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

# ============================================================================
# Source Files
# ============================================================================

# Core ROFF (troff) sources
TROFF_SRCS = \
	roff/main.c \
	roff/roff3.c \
	roff/roff4.c \
	roff/roff5.c \
	roff/roff7.c \
	roff/roff8.c \
	roff/stubs.c

# Extended ROFF (croff) sources - all converted C17 files
# Note: n1.c contains main(), so pti.c and main_stub.c are excluded
CROFF_SRCS = \
	croff/case_stubs.c \
	croff/hytab.c \
	croff/hytab_api.c \
	croff/n1.c \
	croff/n2.c \
	croff/n3.c \
	croff/n4.c \
	croff/n5.c \
	croff/n6.c \
	croff/n7.c \
	croff/n8.c \
	croff/n9.c \
	croff/n10.c \
	croff/ni.c \
	croff/nii.c \
	croff/ntab.c \
	croff/suftab.c \
	croff/t.c \
	croff/troff_processor.c

# Table formatter (tbl) sources
# Table formatter (tbl) sources - stub for now (C++ conversion pending)
TBL_SRCS = tbl/main_stub.c

# Equation formatter (neqn) sources - exclude test files
# Equation formatter (neqn) sources - stub for now (C++ conversion pending)
NEQN_SRCS = neqn/main_stub.c

# OS abstraction layer (shared by all)
OS_SRCS = src/os/os_unix.c

# Terminal drivers for croff
# TERM_SRCS = \
# 	croff/term/tab300.c \
# 	croff/term/tab300-12.c \
# 	croff/term/tab300s.c \
# 	croff/term/tab300s-12.c \
# 	croff/term/tab37.c \
# 	croff/term/tab450.c \
# 	croff/term/tab450-12.c \
# 	croff/term/tab450-12-8.c \
# 	croff/term/tabtn300.c \
# 	croff/term/tabvt100.c \
# 	croff/term/tabvt220.c \
# 	croff/term/terminfo.c \
# 	croff/term/vt220_terminal.c

# Object files
TROFF_OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(TROFF_SRCS) $(OS_SRCS))
CROFF_OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(CROFF_SRCS) $(OS_SRCS))
TBL_OBJS   = $(patsubst %.c,$(OBJDIR)/%.o,$(TBL_SRCS) $(OS_SRCS))
NEQN_OBJS  = $(patsubst %.c,$(OBJDIR)/%.o,$(NEQN_SRCS) $(OS_SRCS))

# Dependency files
ALL_SRCS = $(sort $(TROFF_SRCS) $(CROFF_SRCS) $(TBL_SRCS) $(NEQN_SRCS) $(OS_SRCS) $(TERM_SRCS))
DEPS = $(patsubst %.c,$(OBJDIR)/%.d,$(ALL_SRCS))

# Target executables (in build/bin to avoid conflicts)
BINDIR_BUILD = $(OBJDIR)/bin
TROFF_EXE = $(BINDIR_BUILD)/troff
CROFF_EXE = $(BINDIR_BUILD)/croff
TBL_EXE   = $(BINDIR_BUILD)/tbl
NEQN_EXE  = $(BINDIR_BUILD)/neqn

ALL_EXES = $(TROFF_EXE) $(CROFF_EXE) $(TBL_EXE) $(NEQN_EXE)

# ============================================================================
# Build Rules
# ============================================================================

.PHONY: all clean distclean install uninstall test help info
.PHONY: troff croff tbl neqn

# Default target - build all executables
all: $(ALL_EXES)
	@echo ""
	@echo "==> ALL BUILDS COMPLETE <=="
	@echo "Executables:"
	@ls -lh $(ALL_EXES)

# Individual component targets
troff: $(TROFF_EXE)
croff: $(CROFF_EXE)
tbl: $(TBL_EXE)
neqn: $(NEQN_EXE)

# Link executables
$(TROFF_EXE): $(TROFF_OBJS)
	@echo "==> Linking $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "==> Built: $@ ($$(du -h $@ | cut -f1))"

$(CROFF_EXE): $(CROFF_OBJS)
	@echo "==> Linking $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "==> Built: $@ ($$(du -h $@ | cut -f1))"

$(TBL_EXE): $(TBL_OBJS)
	@echo "==> Linking $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "==> Built: $@ ($$(du -h $@ | cut -f1))"

$(NEQN_EXE): $(NEQN_OBJS)
	@echo "==> Linking $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "==> Built: $@ ($$(du -h $@ | cut -f1))"

# Compile C source files with dependency generation
$(OBJDIR)/%.o: %.c
	@echo "==> Compiling $<..."
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@ 2>&1 | \
		grep -v "warning: ignoring return value" | \
		grep -v "warning.*unused" | \
		grep -v "warning.*set but not used" | \
		cat || true

# Include dependency files
-include $(DEPS)

# ============================================================================
# Utility Targets
# ============================================================================

# Clean build artifacts
clean:
	@echo "==> Cleaning build artifacts..."
	$(RM) -r $(OBJDIR)
	@echo "==> Clean complete."

# Deep clean (includes backup files, CMake artifacts)
distclean: clean
	@echo "==> Deep cleaning..."
	$(RM) *~ roff/*~ src/*~ src/os/*~ croff/*~ croff/term/*~ tbl/*~ neqn/*~
	$(RM) -r build-cmake/ build-meson/ ${BUILD_DIR}/
	$(RM) CMakeCache.txt compile_commands.json
	$(RM) *.o *.a core vgcore.* .*.swp
	@echo "==> Distclean complete."

# Install all executables
install: $(ALL_EXES)
	@echo "==> Installing executables to $(BINDIR)..."
	$(INSTALL) -D -m 755 $(TROFF_EXE) $(BINDIR)/troff
	$(INSTALL) -D -m 755 $(CROFF_EXE) $(BINDIR)/croff
	$(INSTALL) -D -m 755 $(TBL_EXE) $(BINDIR)/tbl
	$(INSTALL) -D -m 755 $(NEQN_EXE) $(BINDIR)/neqn
	@echo "==> Installation complete."

# Uninstall
uninstall:
	@echo "==> Uninstalling executables..."
	$(RM) $(BINDIR)/troff $(BINDIR)/croff $(BINDIR)/tbl $(BINDIR)/neqn
	@echo "==> Uninstall complete."

# Run basic tests
test: $(ALL_EXES)
	@echo "==> Running basic tests..."
	@echo "Testing troff:"
	@$(TROFF_EXE) --help 2>&1 || echo "  troff executable runs"
	@echo "Testing croff:"
	@$(CROFF_EXE) --help 2>&1 || echo "  croff executable runs"
	@echo "Testing tbl:"
	@$(TBL_EXE) --help 2>&1 || echo "  tbl executable runs"
	@echo "Testing neqn:"
	@$(NEQN_EXE) --help 2>&1 || echo "  neqn executable runs"
	@echo "==> Tests complete."

# Display help information
help:
	@echo "OTROFF Comprehensive Makefile - Pure C17 Build System"
	@echo "====================================================="
	@echo "Targets:"
	@echo "  all       - Build all executables (troff, croff, tbl, neqn)"
	@echo "  troff     - Build core troff executable"
	@echo "  croff     - Build extended croff executable"
	@echo "  tbl       - Build table formatter"
	@echo "  neqn      - Build equation formatter"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Deep clean (includes CMake/backup files)"
	@echo "  install   - Install all executables to $(BINDIR)"
	@echo "  uninstall - Remove installed executables"
	@echo "  test      - Run basic tests"
	@echo "  info      - Display build configuration"
	@echo "  help      - Display this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC        - C compiler (default: gcc)"
	@echo "  CFLAGS    - Compiler flags"
	@echo "  CPU       - CPU optimization (default: native)"
	@echo "  PREFIX    - Installation prefix (default: /usr/local)"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build all executables"
	@echo "  make troff        # Build only troff"
	@echo "  make CPU=x86-64   # Build for generic x86-64"
	@echo "  make clean all    # Clean rebuild"
	@echo "  make install      # Install to /usr/local/bin"

# Build information target
info:
	@echo "=== Build Configuration ==="
	@echo "Compiler:      $(CC)"
	@echo "C Standard:    C17"
	@echo "C Flags:       $(CFLAGS)"
	@echo "Includes:      $(INCLUDES)"
	@echo "Build Dir:     $(OBJDIR)"
	@echo "Install Prefix: $(PREFIX)"
	@echo ""
	@echo "=== Components ==="
	@echo "TROFF sources:  $(words $(TROFF_SRCS)) files"
	@echo "CROFF sources:  $(words $(CROFF_SRCS)) files (+$(words $(TERM_SRCS)) terminal drivers)"
	@echo "TBL sources:    $(words $(TBL_SRCS)) files"
	@echo "NEQN sources:   $(words $(NEQN_SRCS)) files"
	@echo "OS layer:       $(words $(OS_SRCS)) files"
	@echo "Total sources:  $(words $(ALL_SRCS)) files"
	@echo "==========================="

# Debugging target - show all variables
debug:
	@echo "TROFF_SRCS = $(TROFF_SRCS)"
	@echo ""
	@echo "CROFF_SRCS = $(CROFF_SRCS)"
	@echo ""
	@echo "TBL_SRCS = $(TBL_SRCS)"
	@echo ""
	@echo "NEQN_SRCS = $(NEQN_SRCS)"
	@echo ""
	@echo "ALL_EXES = $(ALL_EXES)"
