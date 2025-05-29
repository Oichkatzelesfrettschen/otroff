CC ?= cc
CPU ?= native

# Map CPU values to 64-bit -march options
ifeq ($(CPU),x86-64)
    MARCH := x86-64
else ifeq ($(CPU),haswell)
    MARCH := haswell
else
    MARCH := $(CPU)
endif

CFLAGS ?= -std=gnu89 -Wall -O2 -march=$(MARCH)
LDFLAGS ?=

# Collect source files across the project.  Only the modern C
# replacements located under `roff/` are built.
# Source files for each project directory
ROFF_SRC := $(filter-out roff/roff1.c,$(wildcard roff/*.c)) roff/roff1_full.c
CROFF_SRC := $(wildcard croff/*.c)
TBL_SRC   := $(wildcard tbl/*.c)
NEQN_SRC  := $(wildcard neqn/*.c)

# Device driver sources for troff
CROFF_TERMS ?= $(wildcard croff/term/tab*.c)

OBJDIR := build

# Map source files to objects inside $(OBJDIR)
ROFF_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(ROFF_SRC))
CROFF_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(CROFF_SRC))
TBL_OBJ   := $(patsubst %.c,$(OBJDIR)/%.o,$(TBL_SRC))
NEQN_OBJ  := $(patsubst %.c,$(OBJDIR)/%.o,$(NEQN_SRC))
CROFF_TERM_OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(CROFF_TERMS))

ALL_OBJ := $(ROFF_OBJ) $(CROFF_OBJ) $(TBL_OBJ) $(NEQN_OBJ) $(CROFF_TERM_OBJ)


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

$(OBJDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean croff tbl neqn roff
