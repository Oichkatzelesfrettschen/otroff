#pragma once

// Enforce C++23 compilation
#if __cplusplus < 202302L
    #error "This code requires C++23 or later. Use -std=c++23 or equivalent."
#endif

// Disable C compatibility
#ifdef __STDC__
    #error "This is pure C++23 code. Do not compile with a C compiler."
#endif

// Platform checks for C++23 features
#ifndef __cpp_concepts
    #error "C++23 concepts support required"
#endif

#ifndef __cpp_modules
    // Modules are optional but preferred
#endif

#ifndef __cpp_consteval
    #error "C++23 consteval support required"
#endif

// Force strict C++23 mode
#define PURE_CPP23_ONLY

// Modern C++23 includes - replace all C headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <span>
#include <optional>
#include <expected>
#include <memory>
#include <ranges>
#include <algorithm>
#include <format>
#include <concepts>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <source_location>
#include <type_traits>
#include <utility>
#include <variant>
#include <tuple>
#include <bitset>
#include <regex>
#include <locale>
#include <numbers>
#include <bit>
#include <compare>
#include <coroutine>

namespace roff {

// Core type aliases for modern C++23
using byte_t = std::byte;
using size_type = std::size_t;
using string = std::string;
using string_view = std::string_view;
using path = std::filesystem::path;

template<typename T>
using optional = std::optional<T>;

template<typename T, typename E = std::error_code>
using expected = std::expected<T, E>;

template<typename T>
using unique_ptr = std::unique_ptr<T>;

template<typename T>
using shared_ptr = std::shared_ptr<T>;

template<typename T>
using span = std::span<T>;

// Compiler attribute macros (C++23 style)
#if defined(__clang__) || defined(__GNUC__)
    #define ROFF_UNUSED [[maybe_unused]]
    #define ROFF_NODISCARD [[nodiscard]]
    #define ROFF_NORETURN [[noreturn]]
    #define ROFF_DEPRECATED [[deprecated]]
    #define ROFF_FORCE_INLINE [[gnu::always_inline]] inline
    #define ROFF_COLD [[gnu::cold]]
    #define ROFF_HOT [[gnu::hot]]
#else
    #define ROFF_UNUSED [[maybe_unused]]
    #define ROFF_NODISCARD [[nodiscard]]
    #define ROFF_NORETURN [[noreturn]]
    #define ROFF_DEPRECATED [[deprecated]]
    #define ROFF_FORCE_INLINE inline
    #define ROFF_COLD
    #define ROFF_HOT
#endif

// Error handling framework
enum class ErrorCode : std::uint32_t {
    Success = 0,
    FileNotFound = 1000,
    InvalidArgument = 1001,
    BufferOverflow = 1002,
    UnknownCommand = 1003,
    PermissionDenied = 1004,
    InternalError = 1005,
    ParseError = 1006,
    OutOfMemory = 1007,
    InvalidState = 1008,
    Timeout = 1009
};

class RoffException : public std::exception {
private:
    ErrorCode code_;
    string message_;
    std::source_location location_;

public:
    RoffException(ErrorCode code, string_view message,
                  std::source_location location = std::source_location::current())
        : code_(code), message_(message), location_(location) {}

    ROFF_NODISCARD const char* what() const noexcept override {
        return message_.c_str();
    }

    ROFF_NODISCARD ErrorCode code() const noexcept { return code_; }
    ROFF_NODISCARD const std::source_location& location() const noexcept { return location_; }
};

template<typename T>
using Result = expected<T, ErrorCode>;

// Core constants used across ROFF modules
namespace constants {
    // Buffer sizes
    inline constexpr size_type INPUT_BUFFER_SIZE = 512;
    inline constexpr size_type OUTPUT_BUFFER_SIZE = 128;
    inline constexpr size_type STRING_BUFFER_SIZE = 400;
    inline constexpr size_type LINE_BUFFER_SIZE = 256;
    inline constexpr size_type MACRO_BUFFER_SIZE = 1024;

    // Limits
    inline constexpr size_type MAX_TABS = 20;
    inline constexpr size_type MAX_FILES = 10;
    inline constexpr size_type MAX_INCLUDE_DEPTH = 8;
    inline constexpr size_type MAX_MACRO_DEPTH = 16;
    inline constexpr size_type MAX_LINE_LENGTH = 1024;
    inline constexpr size_type MAX_PAGE_LENGTH = 66;

    // Default values
    inline constexpr int DEFAULT_PAGE_LENGTH = 66;
    inline constexpr int DEFAULT_LINE_LENGTH = 65;
    inline constexpr int DEFAULT_INDENT = 0;
    inline constexpr int DEFAULT_TAB_SIZE = 8;
    inline constexpr int MAX_PAGE_NUMBER = 32767;

    // Character constants
    inline constexpr char CONTROL_CHAR = '.';
    inline constexpr char ESCAPE_CHAR = '\\';
    inline constexpr char PREFIX_CHAR = '\033';
    inline constexpr char COMMENT_CHAR = '\"';
    inline constexpr char SPACE_CHAR = ' ';
    inline constexpr char TAB_CHAR = '\t';
    inline constexpr char NEWLINE_CHAR = '\n';

    // File paths
    inline constexpr string_view TEMP_DIR = "/tmp";
    inline constexpr string_view SUFFIX_TABLE_PATH = "/usr/lib/suftab";
    inline constexpr string_view TTY_DEVICE_PREFIX = "/dev/tty";
}

// Strong typing for ROFF concepts
enum class ControlChar : char {
    Dot = constants::CONTROL_CHAR,
    Escape = constants::ESCAPE_CHAR,
    Prefix = constants::PREFIX_CHAR,
    Comment = constants::COMMENT_CHAR
};

enum class ProcessingMode : std::uint8_t {
    Normal = 0,
    Stop = 1,
    HighSpeed = 2,
    Debug = 3
};

enum class TextAlignment : std::uint8_t {
    Left = 0,
    Right = 1,
    Center = 2,
    Justify = 3
};

enum class FillMode : std::uint8_t {
    Fill = 0,
    NoFill = 1
};

enum class UnderlineMode : std::uint8_t {
    None = 0,
    Continuous = 1,
    Words = 2
};

// Character classification concepts
template<typename T>
concept CharacterType = std::same_as<T, char> || std::same_as<T, unsigned char> || std::same_as<T, signed char>;

template<typename T>
concept StringLike = std::convertible_to<T, string_view>;

// Safe character operations
namespace char_ops {
    ROFF_NODISCARD constexpr bool is_control(char c) noexcept {
        return c < ' ' || c > '~';
    }

    ROFF_NODISCARD constexpr bool is_printable(char c) noexcept {
        return c >= ' ' && c <= '~';
    }

    ROFF_NODISCARD constexpr bool is_whitespace(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    ROFF_NODISCARD constexpr bool is_newline(char c) noexcept {
        return c == '\n' || c == '\r';
    }

    ROFF_NODISCARD constexpr bool is_tab(char c) noexcept {
        return c == '\t';
    }

    ROFF_NODISCARD constexpr int display_width(char c) noexcept {
        return is_control(c) ? 0 : 1;
    }

    ROFF_NODISCARD constexpr char to_lower(char c) noexcept {
        return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
    }

    ROFF_NODISCARD constexpr char to_upper(char c) noexcept {
        return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
    }
}

// Safe buffer management template
template<size_type Size>
class SafeBuffer {
private:
    std::array<char, Size> data_{};
    size_type position_{0};

public:
    constexpr SafeBuffer() = default;

    ROFF_NODISCARD constexpr span<char> available_space() noexcept {
        return span{data_.data() + position_, Size - position_};
    }

    ROFF_NODISCARD constexpr span<const char> used_space() const noexcept {
        return span{data_.data(), position_};
    }

    ROFF_NODISCARD constexpr string_view as_string_view() const noexcept {
        return string_view{data_.data(), position_};
    }

    constexpr bool append(char c) noexcept {
        if (position_ >= Size) return false;
        data_[position_++] = c;
        return true;
    }

    constexpr bool append(span<const char> data) noexcept {
        if (position_ + data.size() > Size) return false;
        std::ranges::copy(data, data_.begin() + position_);
        position_ += data.size();
        return true;
    }

    constexpr bool append(string_view str) noexcept {
        return append(span<const char>{str.data(), str.size()});
    }

    constexpr void clear() noexcept {
        position_ = 0;
        data_.fill('\0');
    }

    ROFF_NODISCARD constexpr size_type size() const noexcept { return position_; }
    ROFF_NODISCARD constexpr size_type capacity() const noexcept { return Size; }
    ROFF_NODISCARD constexpr bool empty() const noexcept { return position_ == 0; }
    ROFF_NODISCARD constexpr bool full() const noexcept { return position_ == Size; }
    ROFF_NODISCARD constexpr size_type remaining() const noexcept { return Size - position_; }

    ROFF_NODISCARD constexpr char operator[](size_type index) const noexcept {
        return data_[index];
    }

    ROFF_NODISCARD constexpr char& operator[](size_type index) noexcept {
        return data_[index];
    }
};

// File handling utilities
namespace file_utils {
    ROFF_NODISCARD inline bool exists(const path& file_path) noexcept {
        std::error_code ec;
        return std::filesystem::exists(file_path, ec);
    }

    ROFF_NODISCARD inline bool is_readable(const path& file_path) noexcept {
        std::error_code ec;
        auto status = std::filesystem::status(file_path, ec);
        return !ec && (status.permissions() & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
    }

    ROFF_NODISCARD inline bool is_writable(const path& file_path) noexcept {
        std::error_code ec;
        auto status = std::filesystem::status(file_path, ec);
        return !ec && (status.permissions() & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
    }

    ROFF_NODISCARD inline size_type file_size(const path& file_path) noexcept {
        std::error_code ec;
        auto size = std::filesystem::file_size(file_path, ec);
        return ec ? 0 : size;
    }

    inline path create_temp_file(string_view prefix = "roff") {
        auto temp_dir = std::filesystem::temp_directory_path();
        auto temp_file = temp_dir / std::format("{}_{:x}", prefix,
                                               std::chrono::steady_clock::now().time_since_epoch().count());
        return temp_file;
    }
}

// String utilities
namespace string_utils {
    ROFF_NODISCARD inline string trim_left(string_view str) {
        auto start = str.find_first_not_of(" \t\n\r");
        return start == string_view::npos ? string{} : string{str.substr(start)};
    }

    ROFF_NODISCARD inline string trim_right(string_view str) {
        auto end = str.find_last_not_of(" \t\n\r");
        return end == string_view::npos ? string{} : string{str.substr(0, end + 1)};
    }

    ROFF_NODISCARD inline string trim(string_view str) {
        return trim_left(trim_right(str));
    }

    ROFF_NODISCARD inline std::vector<string> split(string_view str, char delimiter = ' ') {
        std::vector<string> result;
        size_type start = 0;
        size_type end = str.find(delimiter);

        while (end != string_view::npos) {
            if (end != start) {
                result.emplace_back(str.substr(start, end - start));
            }
            start = end + 1;
            end = str.find(delimiter, start);
        }

        if (start < str.length()) {
            result.emplace_back(str.substr(start));
        }

        return result;
    }

    ROFF_NODISCARD inline bool starts_with(string_view str, string_view prefix) noexcept {
        return str.starts_with(prefix);
    }

    ROFF_NODISCARD inline bool ends_with(string_view str, string_view suffix) noexcept {
        return str.ends_with(suffix);
    }

    ROFF_NODISCARD inline bool contains(string_view str, string_view substr) noexcept {
        return str.find(substr) != string_view::npos;
    }
}

// Number parsing utilities
namespace parse_utils {
    ROFF_NODISCARD inline optional<int> parse_int(string_view str) noexcept {
        int result;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
        return ec == std::errc{} ? optional<int>{result} : std::nullopt;
    }

    ROFF_NODISCARD inline optional<double> parse_double(string_view str) noexcept {
        try {
            size_type pos;
            double result = std::stod(string{str}, &pos);
            return pos == str.size() ? optional<double>{result} : std::nullopt;
        } catch (...) {
            return std::nullopt;
        }
    }

    ROFF_NODISCARD inline optional<size_type> parse_size(string_view str) noexcept {
        size_type result;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
        return ec == std::errc{} ? optional<size_type>{result} : std::nullopt;
    }
}

// Debug and logging utilities
namespace debug {
    inline void log_info(string_view message, std::source_location location = std::source_location::current()) {
        std::cerr << std::format("[INFO] {}:{} {}\n",
                                location.file_name(), location.line(), message);
    }

    inline void log_warning(string_view message, std::source_location location = std::source_location::current()) {
        std::cerr << std::format("[WARN] {}:{} {}\n",
                                location.file_name(), location.line(), message);
    }

    inline void log_error(string_view message, std::source_location location = std::source_location::current()) {
        std::cerr << std::format("[ERROR] {}:{} {}\n",
                                location.file_name(), location.line(), message);
    }

    ROFF_NORETURN inline void fatal_error(string_view message,
                                          std::source_location location = std::source_location::current()) {
        std::cerr << std::format("[FATAL] {}:{} {}\n",
                                location.file_name(), location.line(), message);
        std::abort();
    }
}

// Memory management utilities
namespace memory {
    template<typename T, typename... Args>
    ROFF_NODISCARD unique_ptr<T> make_unique(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    ROFF_NODISCARD shared_ptr<T> make_shared(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

// Time utilities
namespace time_utils {
    ROFF_NODISCARD inline std::chrono::system_clock::time_point now() noexcept {
        return std::chrono::system_clock::now();
    }

    ROFF_NODISCARD inline string format_time(const std::chrono::system_clock::time_point& time_point,
                                            string_view format = "%Y-%m-%d %H:%M:%S") {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        auto tm = *std::localtime(&time_t);

        std::ostringstream oss;
        oss << std::put_time(&tm, format.data());
        return oss.str();
    }
}

// Global type aliases for common ROFF data structures
using InputBuffer = SafeBuffer<constants::INPUT_BUFFER_SIZE>;
using OutputBuffer = SafeBuffer<constants::OUTPUT_BUFFER_SIZE>;
using StringBuffer = SafeBuffer<constants::STRING_BUFFER_SIZE>;
using LineBuffer = SafeBuffer<constants::LINE_BUFFER_SIZE>;
using MacroBuffer = SafeBuffer<constants::MACRO_BUFFER_SIZE>;

} // namespace roff

// Compatibility layer for legacy code transition
namespace roff::compat {
    // Legacy type names for gradual migration
    using ROFF_UNUSED_TYPE = int ROFF_UNUSED;

    // Legacy constants
    inline constexpr int IBUF_SIZE = constants::INPUT_BUFFER_SIZE;
    inline constexpr int OBUF_SIZE = constants::OUTPUT_BUFFER_SIZE;
    inline constexpr int SSIZE = constants::STRING_BUFFER_SIZE;
    inline constexpr int MAXFILES = constants::MAX_FILES;

    // Legacy character constants
    inline constexpr char CC_CHAR = constants::CONTROL_CHAR;
    inline constexpr char ESC_CHAR = constants::ESCAPE_CHAR;
    inline constexpr char PREFIX_CHAR = constants::PREFIX_CHAR;
}

// Global assertions for development
#ifdef ROFF_DEBUG
    #define ROFF_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                roff::debug::fatal_error(std::format("Assertion failed: {} - {}", #condition, message)); \
            } \
        } while(0)
#else
    #define ROFF_ASSERT(condition, message) ((void)0)
#endif

// Performance profiling macros
#ifdef ROFF_PROFILE
    #define ROFF_PROFILE_SCOPE(name) \
        auto _profile_start = std::chrono::high_resolution_clock::now(); \
        auto _profile_guard = std::unique_ptr<void, std::function<void(void*)>>( \
            nullptr, [=](void*) { \
                auto _profile_end = std::chrono::high_resolution_clock::now(); \
                auto _profile_duration = std::chrono::duration_cast<std::chrono::microseconds>(_profile_end - _profile_start); \
                roff::debug::log_info(std::format("Profile {}: {}μs", name, _profile_duration.count())); \
            });
#else
    #define ROFF_PROFILE_SCOPE(name) ((void)0)
#endif
