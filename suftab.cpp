#include "cxx17_scaffold.hpp"
#include "suftab.hpp"

#include <array>
#include <format>
#include <print>

namespace roff::data {

// Suffix table data - modernized array declaration
constexpr std::array<char, SUFTAB_SIZE> suftab = {
    // ...existing code...
};

// Function to print suffix table information.
void print_suftab_info() {
    std::print("Suffix table size: {} bytes\n", suftab.size());
    std::print(
        "First few bytes: {}, {}, {}, {}\n",
        static_cast<unsigned char>(suftab[0]),
        static_cast<unsigned char>(suftab[1]),
        static_cast<unsigned char>(suftab[2]),
        static_cast<unsigned char>(suftab[3]));
}

} // namespace roff::data
