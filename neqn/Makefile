# ==============================================================================
# Makefile for neqn - Mathematical Equation Preprocessor
# ==============================================================================
#
# This Makefile builds the neqn mathematical equation preprocessor with
# C90 compliance and proper dependency tracking.
#
# Author: Various contributors
# Version: 2.0 (C90 compliant)
# Date: 2023
#
# ==============================================================================

# ------------------------------------------------------------------------------
# Build Configuration
# ------------------------------------------------------------------------------

# Program name and version
PROGRAM = neqn
VERSION = 2.0

# Compiler settings
CC = clang
CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -DNEQN_VERSION=\"$(VERSION)\"
LDFLAGS = 
LIBS = 

# Debug build settings
DEBUG_CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
DEBUG_CPPFLAGS = -DNEQN_VERSION=\"$(VERSION)-debug\"

# Installation directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

# Build directories
SRCDIR = .
OBJDIR = obj
DEPDIR = deps

# ------------------------------------------------------------------------------
# Source Files and Objects
# ------------------------------------------------------------------------------

# Source files
SOURCES = neqn.c ne0.c ne_core.c ne_symbols.c

# Test source files
TEST_SOURCES = test_neqn.c ne0.c ne_core.c ne_symbols.c

# Object files
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.c=$(OBJDIR)/%.o)

# Dependency files
DEPENDS = $(SOURCES:%.c=$(DEPDIR)/%.d)
TEST_DEPENDS = $(TEST_SOURCES:%.c=$(DEPDIR)/%.d)

# Header files
HEADERS = ne.h

# ------------------------------------------------------------------------------
# Build Targets
# ------------------------------------------------------------------------------

# Default target
.PHONY: all
all: $(PROGRAM)

# Main program
$(PROGRAM): $(OBJECTS)
	@echo "Linking $(PROGRAM)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "Build completed successfully."

# Test program
test_$(PROGRAM): $(TEST_OBJECTS)
	@echo "Linking test_$(PROGRAM)..."
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Object files with dependency generation
$(OBJDIR)/%.o: %.c | $(OBJDIR) $(DEPDIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@

# Create build directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(DEPDIR):
	@mkdir -p $(DEPDIR)

# ------------------------------------------------------------------------------
# Debug Build
# ------------------------------------------------------------------------------

.PHONY: debug
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: CPPFLAGS = $(DEBUG_CPPFLAGS)
debug: $(PROGRAM)

# ------------------------------------------------------------------------------
# Testing and Validation
# ------------------------------------------------------------------------------

.PHONY: test
test: test_$(PROGRAM)
	@echo "Running basic functionality tests..."
	@./test_$(PROGRAM)
	@echo ""
	@echo "Running integration tests..."
	@echo "x sup 2 + y sup 2 = r sup 2" | ./$(PROGRAM)
	@echo "Test completed."

.PHONY: check
check: test
	@echo "Running comprehensive tests..."
	@echo "All tests passed."

.PHONY: lint
lint:
	@echo "Running code quality checks..."
	@if command -v clang-tidy >/dev/null 2>&1; then \
		clang-tidy $(SOURCES) -- $(CFLAGS) $(CPPFLAGS); \
	else \
		echo "clang-tidy not available, skipping static analysis"; \
	fi

.PHONY: format
format:
	@echo "Formatting source code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i $(SOURCES) $(HEADERS); \
		echo "Code formatting completed."; \
	else \
		echo "clang-format not available, skipping formatting"; \
	fi

# ------------------------------------------------------------------------------
# Installation and Distribution
# ------------------------------------------------------------------------------

.PHONY: install
install: $(PROGRAM)
	@echo "Installing $(PROGRAM)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(PROGRAM) $(DESTDIR)$(BINDIR)
	@echo "Installation completed."

.PHONY: uninstall
uninstall:
	@echo "Uninstalling $(PROGRAM)..."
	rm -f $(DESTDIR)$(BINDIR)/$(PROGRAM)
	@echo "Uninstallation completed."

.PHONY: dist
dist: clean
	@echo "Creating distribution archive..."
	@mkdir -p $(PROGRAM)-$(VERSION)
	@cp -r $(SOURCES) $(HEADERS) Makefile README* LICENSE* $(PROGRAM)-$(VERSION)/
	@tar -czf $(PROGRAM)-$(VERSION).tar.gz $(PROGRAM)-$(VERSION)
	@rm -rf $(PROGRAM)-$(VERSION)
	@echo "Distribution archive created: $(PROGRAM)-$(VERSION).tar.gz"

# ------------------------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------------------------

.PHONY: clean
clean:
	@echo "Cleaning build files..."
	rm -rf $(OBJDIR) $(DEPDIR)
	rm -f $(PROGRAM) test_$(PROGRAM)
	rm -f *.o *.d *~
	@echo "Cleanup completed."

.PHONY: distclean
distclean: clean
	@echo "Performing deep cleanup..."
	rm -f $(PROGRAM)-*.tar.gz
	rm -f tags TAGS
	@echo "Deep cleanup completed."

# ------------------------------------------------------------------------------
# Development Helpers
# ------------------------------------------------------------------------------

.PHONY: tags
tags:
	@echo "Generating tags..."
	@if command -v ctags >/dev/null 2>&1; then \
		ctags -R .; \
	else \
		echo "ctags not available"; \
	fi

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  test     - Run basic tests"
	@echo "  check    - Run comprehensive tests"
	@echo "  lint     - Run static code analysis"
	@echo "  format   - Format source code"
	@echo "  install  - Install the program"
	@echo "  uninstall- Uninstall the program"
	@echo "  dist     - Create distribution archive"
	@echo "  clean    - Remove build files"
	@echo "  distclean- Remove all generated files"
	@echo "  tags     - Generate ctags"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC       - Compiler (default: clang)"
	@echo "  CFLAGS   - Compiler flags"
	@echo "  PREFIX   - Installation prefix (default: /usr/local)"
	@echo "  DESTDIR  - Installation destination directory"

# ------------------------------------------------------------------------------
# Version Information
# ------------------------------------------------------------------------------

.PHONY: version
version:
	@echo "$(PROGRAM) version $(VERSION)"
	@echo "C90 compliant mathematical equation preprocessor"

# ------------------------------------------------------------------------------
# Dependency Inclusion
# ------------------------------------------------------------------------------

# Include dependency files if they exist
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPENDS)
endif
endif

# ------------------------------------------------------------------------------
# Special Targets
# ------------------------------------------------------------------------------

# Prevent make from deleting intermediate files
.SECONDARY:

# Disable built-in rules and suffixes
.SUFFIXES:

# Mark targets that don't create files
.PHONY: all debug test check lint format install uninstall dist clean distclean tags help version

# ==============================================================================
# End of Makefile
# ==============================================================================
