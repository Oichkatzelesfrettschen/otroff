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

# Collect source files across the project
SRC_C := $(wildcard croff/*.c neqn/*.c tbl/*.c)
OBJDIR := build
OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_C))

all: $(OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
