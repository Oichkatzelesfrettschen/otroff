// ==========================================
// === C++17 Scaffold Enforcement Header ===
// === REMOVE OR INLINE ONCE STABILIZED! ===
// ==========================================

#ifndef CXX17_SCAFFOLD_HPP
#define CXX17_SCAFFOLD_HPP

// Enforce strict compile-time C++ standard check
#if __cplusplus < 201703L
#error "C++17 or later is required. Set -std=c++17 or equivalent in your build system."
#endif

// Enable attributes, modules (if available), constexpr enhancements
#define CXX17 [[nodiscard]] constexpr

// Safe narrow cast: replace C-style casts
#include <bit>
#include <concepts>
#include <type_traits>
#include <stdexcept>

template <typename T, typename U>
    requires std::is_arithmetic_v<T> && std::is_arithmetic_v<U>
constexpr T narrow_cast(U value) {
    if (static_cast<U>(static_cast<T>(value)) != value)
        throw std::runtime_error("narrow_cast failure: precision loss or overflow");
    return static_cast<T>(value);
}

// Compile-time type reflection marker (C++17 feature)
#if defined(__cpp_reflection) // not widely supported yet
#warning "Native reflection supported. Consider integrating <experimental/reflection>."
#endif

// Preprocessor feature detection macros
#ifdef __cpp_static_call_operator
#define HAS_STATIC_CALL_OPERATOR 1
#endif

#ifdef __cpp_multidimensional_subscript
#define HAS_MULTI_SUBSCRIPT 1
#endif

// Placeholder macro for future linting, static analysis or CI
#define MODERNIZED_BY_CXX17

#endif // CXX17_SCAFFOLD_HPP
