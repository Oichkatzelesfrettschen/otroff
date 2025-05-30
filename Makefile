# Modern Makefile for otroff using C++17

# Compiler configuration
CXX ?= clang++
CPU ?= native

# Map CPU value to -march option
ifeq ($(CPU),x86-64)
MARCH := x86-64
else ifeq ($(CPU),haswell)
MARCH := haswell
else
MARCH := $(CPU)
endif

# Compiler and linker flags
CXXFLAGS ?= -std=c++17 -Wall -O2 -march=$(MARCH)
LDFLAGS ?= -fopenmp=libgomp

# Source file discovery
ROFF_SRC := $(sort $(wildcard roff/*.c roff/*.cpp))
OS_SRC := src/os/os_unix.cpp
STUBS_SRC := src/stubs.cpp
CROFF_SRC := $(filter-out croff/test_%.cpp,$(sort $(wildcard croff/*.c croff/*.cpp)))
TBL_SRC := $(sort $(wildcard tbl/*.c tbl/*.cpp))
NEQN_SRC := $(sort $(wildcard neqn/*.c neqn/*.cpp))
CROFF_TERMS :=

# Build directory
OBJDIR := build

# Object file lists
ROFF_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(ROFF_SRC))
CROFF_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CROFF_SRC))
TBL_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(TBL_SRC))
NEQN_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(NEQN_SRC))
OS_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(OS_SRC))
STUBS_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(STUBS_SRC))
CROFF_TERM_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CROFF_TERMS))

# Aggregated object lists
TROFF_OBJ := $(ROFF_OBJ) $(OS_OBJ) $(STUBS_OBJ)
ALL_OBJ := $(TROFF_OBJ) $(CROFF_OBJ) $(TBL_OBJ) $(NEQN_OBJ) $(CROFF_TERM_OBJ)

# Default target builds troff
all: $(OBJDIR)/troff

$(OBJDIR)/troff: $(TROFF_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Optional component targets
croff: $(CROFF_OBJ) $(CROFF_TERM_OBJ)
tbl: $(TBL_OBJ)
neqn: $(NEQN_OBJ)
roff: $(ROFF_OBJ)

# Generic C++ compile rule
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR)

.PHONY: all clean croff tbl neqn roff test format cmake meson

# CMake build helper
cmake:
	cmake -S . -B build-cmake
	cmake --build build-cmake

# Meson build helper
meson:
	meson setup build-meson --reconfigure || meson setup build-meson
	ninja -C build-meson

# Run Python tests
test:
	pytest -q

# Format all sources with clang-format
format:
	find . \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) -print0 | xargs -0 clang-format -i
