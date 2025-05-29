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
SRC_C := $(wildcard roff/*.c)
OBJDIR := build
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_C))

ifdef USE_SSE
SRC_SSE := roff/sse_memops.S
OBJ += $(patsubst %.S,$(OBJDIR)/%.o,$(SRC_SSE))
endif

all: $(OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
