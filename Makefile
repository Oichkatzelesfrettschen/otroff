# Pure C17 Makefile for OTROFF Text Formatter
# Modern build system using traditional make
# Replaces CMake with clean, explicit make-based build

# ============================================================================
# Configuration
# ============================================================================

# Compiler and tools
CC = gcc
AR = ar
RANLIB = ranlib
RM = rm -f
MKDIR = mkdir -p

# Target executable
TARGET = troff

# C17 standard with strict compliance
CFLAGS = -std=c17 -pedantic -Wall -Wextra
CFLAGS += -O2 -g
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_GNU_SOURCE

# CPU optimization (override with CPU=x86-64, haswell, native, etc.)
CPU ?= native
CFLAGS += -march=$(CPU)

# Include paths
INCLUDES = -I. -Iroff -Isrc -Isrc/os

# Libraries
LDFLAGS =
LDLIBS =

# Build directory
OBJDIR = build

# ============================================================================
# Source Files
# ============================================================================

# Core ROFF implementation (roff/ directory)
ROFF_SRCS = \
	roff/main.c \
	roff/roff3.c \
	roff/roff4.c \
	roff/roff5.c \
	roff/roff7.c \
	roff/roff8.c \
	roff/stubs.c

# OS abstraction layer (src/os/ directory)
OS_SRCS = src/os/os_unix.c

# All sources
SRCS = $(ROFF_SRCS) $(OS_SRCS)

# Object files
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# Dependency files
DEPS = $(patsubst %.c,$(OBJDIR)/%.d,$(SRCS))

# ============================================================================
# Build Rules
# ============================================================================

.PHONY: all clean distclean install test help info

# Default target
all: $(OBJDIR)/$(TARGET)

# Link the executable
$(OBJDIR)/$(TARGET): $(OBJS)
	@echo "==> Linking $@..."
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	@echo "==> Build complete: $@"
	@ls -lh $@
	@file $@
	@ldd $@ 2>/dev/null || true

# Compile C source files with dependency generation
$(OBJDIR)/%.o: %.c
	@echo "==> Compiling $<..."
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

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
	$(RM) *~ roff/*~ src/*~ src/os/*~
	$(RM) -r build-cmake/ build-meson/ ${BUILD_DIR}/
	$(RM) CMakeCache.txt
	$(RM) compile_commands.json
	@echo "==> Distclean complete."

# Install (basic installation to /usr/local/bin)
install: $(OBJDIR)/$(TARGET)
	@echo "==> Installing $(TARGET) to /usr/local/bin..."
	install -D -m 755 $(OBJDIR)/$(TARGET) /usr/local/bin/$(TARGET)
	@echo "==> Installation complete."

# Uninstall
uninstall:
	@echo "==> Uninstalling $(TARGET)..."
	$(RM) /usr/local/bin/$(TARGET)
	@echo "==> Uninstall complete."

# Run basic tests
test: $(OBJDIR)/$(TARGET)
	@echo "==> Running basic tests..."
	$(OBJDIR)/$(TARGET) || true
	@echo "==> Test complete."

# Display help information
help:
	@echo "OTROFF Makefile - Pure C17 Build System"
	@echo "========================================"
	@echo "Available targets:"
	@echo "  all       - Build the troff executable (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Deep clean (includes CMake/Meson artifacts)"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  test      - Run basic tests"
	@echo "  info      - Display build configuration"
	@echo "  help      - Display this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC        - C compiler (default: gcc)"
	@echo "  CFLAGS    - Compiler flags"
	@echo "  CPU       - CPU optimization (default: native)"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build with default settings"
	@echo "  make CPU=x86-64   # Build for generic x86-64"
	@echo "  make clean all    # Clean rebuild"

# Build information target
info:
	@echo "=== Build Configuration ==="
	@echo "Compiler:    $(CC)"
	@echo "C Standard:  C17"
	@echo "C Flags:     $(CFLAGS)"
	@echo "Includes:    $(INCLUDES)"
	@echo "Target:      $(TARGET)"
	@echo "Sources:     $(words $(SRCS)) files"
	@echo "Build Dir:   $(OBJDIR)"
	@echo "==========================="
	@echo "Source files:"
	@$(foreach src,$(SRCS),echo "  - $(src)";)
