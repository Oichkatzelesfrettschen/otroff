/// \file runtime.hpp
/// \brief Helper routines used by roff utilities at runtime.

#pragma once

#include "cxx17_scaffold.hpp"

#include <cstddef>

/* Enable or disable write permission on the controlling terminal. */
void mesg(int enable);

/* Return distance to next 8-column tab stop from a column position. */
[[nodiscard]] int dsp(int column) noexcept;

/* Write buffer contents to stdout and reset the index pointer. */
void flush_output(char *buf, size_t *p) noexcept;
