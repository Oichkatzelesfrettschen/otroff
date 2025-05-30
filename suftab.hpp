/*
 * suftab.h - Suffix table interface
 * Header file for suffix table functionality
 */

#ifndef SUFTAB_H
#define SUFTAB_H
#include "cxx23_scaffold.hpp"
#include <array> // std::array container
#include <span>

/* External declarations */
extern const std::array<char, SUFTAB_SIZE> suftab; // suffix data storage

/* Function prototypes */
void print_suftab_info(void); // display suffix table stats

/* Utility macros */
#define SUFTAB_SIZE 4096

#endif /* SUFTAB_H */
