/*
 * suftab.h - Suffix table interface
 * Header file for suffix table functionality
 */

#ifndef SUFTAB_H
#define SUFTAB_H

#include "cxx23_scaffold.hpp"

#include <array>
#include <span>

namespace roff::data {

constexpr std::size_t SUFTAB_SIZE = 4096;

extern const std::array<char, SUFTAB_SIZE> suftab; // suffix data storage

void print_suftab_info(); // display suffix table stats

} // namespace roff::data

#endif // SUFTAB_H
