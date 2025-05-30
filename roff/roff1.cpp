// Enforce C++17 compilation for maximum compatibility
#if __cplusplus < 201703L
#error "This code requires C++17 or later. Use -std=c++17 or equivalent."
#endif

/**
 * @file roff1.cpp
 * @brief ROFF text formatter - Main driver and core functionality (Modern C++)
 * @author Generated with modern C++ best practices
 * @version 2.0
 * @date 2024
 *
 * @details This implementation provides a complete ROFF text processor using
 * modern C++ features including smart pointers, RAII, and type safety.
 * All C-style code has been eliminated in favor of type-safe, exception-safe
 * modern C++ constructs.
 */

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>

// Forward declarations and error handling
namespace roff {

enum class ErrorCode {
    Success = 0,
    InvalidArgument = 1,
    FileNotFound = 2,
    InternalError = 3,
    OutputError = 4,
    BufferOverflow = 5
};

enum class TextAlignment {
    Left = 0,
    Right = 1,
    Center = 2,
    Both = 3
};

enum class ProcessingMode {
    Normal = 0,
    Stop = 1,
    HighSpeed = 2
};

struct RoffConfig {
    bool fill_mode{true};
    int indent{0};
    int temp_indent{0};
    int line_length{65};
    int page_length{66};
    int start_page{1};
    int end_page{0};
    int centering_lines_count{0};
    bool apply_temp_indent_once{false};
    std::optional<int> previous_indent;
    TextAlignment adjust_mode{TextAlignment::Left};
    ProcessingMode mode{ProcessingMode::Normal};
    
    bool is_valid() const noexcept {
        return line_length > 0 && page_length >= 0 && start_page > 0;
    }
};

class RoffException : public std::exception {
private:
    ErrorCode code_;
    std::string message_;

public:
    RoffException(ErrorCode code, const std::string& message) 
        : code_(code), message_(message) {}
    
    const char* what() const noexcept override { return message_.c_str(); }
    ErrorCode code() const noexcept { return code_; }
    
    struct source_location {
        std::string file_name() const { return "roff1.cpp"; }
        int line() const { return 0; }
    };
    
    source_location location() const { return {}; }
};

namespace constants {
    constexpr char CONTROL_CHAR = '.';
    constexpr char ESCAPE_CHAR = '\\';
    constexpr char PREFIX_CHAR = '%';
    constexpr int DEFAULT_LINE_LENGTH = 65;
    constexpr int DEFAULT_PAGE_LENGTH = 66;
}

namespace debug {
    void log_info(const std::string& msg) {
        std::cerr << "[INFO] " << msg << std::endl;
    }
    void log_warning(const std::string& msg) {
        std::cerr << "[WARN] " << msg << std::endl;
    }
    void log_error(const std::string& msg) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
}

namespace string_utils {
    std::string_view trim(const std::string& str) {
        const auto start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return {};
        const auto end = str.find_last_not_of(" \t\r\n");
        return std::string_view(str).substr(start, end - start + 1);
    }
}

namespace parse_utils {
    std::optional<int> parse_int(std::string_view str) {
        try {
            std::string s(str);
            return std::stoi(s);
        } catch (...) {
            return std::nullopt;
        }
    }
}

class OutputBuffer {
private:
    std::vector<char> buffer_;
    std::size_t capacity_;
    
public:
    explicit OutputBuffer(std::size_t capacity = 8192) 
        : capacity_(capacity) {
        buffer_.reserve(capacity_);
    }
    
    bool append(char c) {
        if (buffer_.size() >= capacity_) return false;
        buffer_.push_back(c);
        return true;
    }
    
    std::string_view used_space() const {
        return std::string_view(buffer_.data(), buffer_.size());
    }
    
    void clear() { buffer_.clear(); }
    bool empty() const { return buffer_.empty(); }
};

namespace engine {

using std::string;
using std::string_view;

/// @brief Simple Result type for error handling
template <typename T>
class Result {
private:
    std::optional<T> value_;
    std::optional<ErrorCode> error_;

public:
    Result(T&& val) : value_(std::move(val)) {}
    Result(const T& val) : value_(val) {}
    Result(ErrorCode err) : error_(err) {}
    
    bool has_value() const { return value_.has_value(); }
    operator bool() const { return has_value(); }
    
    T& operator*() { return *value_; }
    const T& operator*() const { return *value_; }
    
    T* operator->() { return &(*value_); }
    const T* operator->() const { return &(*value_); }
    
    ErrorCode error() const { return error_.value_or(ErrorCode::Success); }
};

template<>
class Result<void> {
private:
    std::optional<ErrorCode> error_;

public:
    Result() = default;
    Result(ErrorCode err) : error_(err) {}
    
    bool has_value() const { return !error_.has_value(); }
    operator bool() const { return has_value(); }
    
    ErrorCode error() const { return error_.value_or(ErrorCode::Success); }
};

// Helper function for starts_with (C++20 feature)
bool starts_with(std::string_view str, char c) {
    return !str.empty() && str[0] == c;
}

bool starts_with(std::string_view str, std::string_view prefix) {
    return str.length() >= prefix.length() && 
            return command_break_line();
        };

        commands_["bp"] = [this](string_view args) -> Result<void> {
            return command_break_page(args);
        };

        commands_["sp"] = [this](string_view args) -> Result<void> {
            return command_space_lines(args);
        };

        // Text formatting commands
        commands_["ce"] = [this](string_view args) -> Result<void> {
            return command_center_lines(args);
        };

        commands_["fi"] = [this](string_view) -> Result<void> {
            config_.fill_mode = true;
            debug::log_info("Fill mode enabled");
            return Result<void>{};
        };

        commands_["nf"] = [this](string_view) -> Result<void> {
            config_.fill_mode = false;
            debug::log_info("Fill mode disabled");
            return Result<void>{};
        };

        // Indentation and spacing commands
        commands_["in"] = [this](string_view args) -> Result<void> {
            return command_set_indent(args);
        };

        commands_["ll"] = [this](string_view args) -> Result<void> {
            return command_set_line_length(args);
        };

        commands_["ti"] = [this](string_view args) -> Result<void> {
            return command_temporary_indent(args);
        };

        commands_["pl"] = [this](string_view args) -> Result<void> {
            return command_set_page_length(args);
        };

        // Text adjustment commands
        commands_["ad"] = [this](string_view args) -> Result<void> {
            return command_adjust_text(args);
        };

        commands_["na"] = [this](string_view) -> Result<void> {
            config_.adjust_mode = TextAlignment::Left;
            debug::log_info("Text adjustment disabled");
            return Result<void>{};
        };

        // File processing commands
        commands_["so"] = [this](string_view args) -> Result<void> {
            return command_source_file(args);
        };

        commands_["nx"] = [this](string_view args) -> Result<void> {
            return command_next_file(args);
        };

        commands_["ex"] = [this](string_view) -> Result<void> {
            debug::log_info("Exit command received");
            exit_requested_ = true;
            return Result<void>{};
        };
    }

    /**
     * @brief Process a single command line argument
     * @param arg The argument to process
     * @return Result indicating success or failure
     */
    Result<void> process_single_argument(string_view arg) {
        if (arg.starts_with('+')) {
            if (auto page = parse_utils::parse_int(arg.substr(1))) {
                config_.start_page = *page;
                return Result<void>{};
            }
            return Result<void>{ErrorCode::InvalidArgument};
        }

        if (arg.starts_with('-')) {
            if (arg == "-s") {
                config_.mode = ProcessingMode::Stop;
                return Result<void>{};
            }
            if (arg == "-h") {
                config_.mode = ProcessingMode::HighSpeed;
                return Result<void>{};
            }
            if (auto page = parse_utils::parse_int(arg.substr(1))) {
                config_.end_page = *page;
                return Result<void>{};
            }
            return Result<void>{ErrorCode::InvalidArgument};
        }

        // Input file
        return add_input_file(arg);
    }

    /**
     * @brief Add an input file to the processing queue
     * @param filename Path to the file to add
     * @param insert_next Whether to insert after current file or append
     * @return Result indicating success or failure
     */
    Result<void> add_input_file(string_view filename, bool insert_next = false) {
        try {
            auto file = std::make_unique<std::ifstream>(string{filename});
            if (!file || !file->is_open()) {
                debug::log_warning("Cannot open input file: " + string{filename});
                return Result<void>{ErrorCode::FileNotFound};
            }

            if (insert_next && current_file_index_ + 1 < input_files_.size()) {
                auto insert_pos = input_files_.begin() + static_cast<std::ptrdiff_t>(current_file_index_ + 1);
                input_files_.insert(insert_pos, std::move(file));
            } else {
                input_files_.push_back(std::move(file));
            }

            debug::log_info("Added input file: " + string{filename});
            return Result<void>{};
        } catch (const std::exception& e) {
            debug::log_error("Exception adding input file " + string{filename} + ": " + e.what());
            return Result<void>{ErrorCode::FileNotFound};
        }
    }

    /**
     * @brief Get the next character from input stream
     * @return Optional character or nullopt if no more input
     */
    std::optional<char> get_next_character() {
        if (exit_requested_) {
            return std::nullopt;
        }

        while (current_file_index_ < input_files_.size()) {
            auto& current_file = input_files_[current_file_index_];

            if (!current_file || !current_file->is_open()) {
                ++current_file_index_;
                continue;
            }

            char c;
            if (current_file->get(c)) {
                return c;
            }

            // EOF or error - move to next file
            current_file->close();
            ++current_file_index_;
        }

        return std::nullopt;
    }

    /**
     * @brief Process a control command sequence
     * @return Result indicating success or failure
     */
    Result<void> process_control_command() {
        // Read command name (2 characters as per ROFF specification)
        auto cmd1 = get_next_character();
        auto cmd2 = get_next_character();

        if (!cmd1 || !cmd2) {
            return Result<void>{ErrorCode::InternalError};
        }

        const string command{*cmd1, *cmd2};

        // Read arguments until newline
        string args_buffer;
        while (auto ch_opt = get_next_character()) {
            if (*ch_opt == '\n')
                break;
            args_buffer += *ch_opt;
        }

        const auto trimmed_args = string_utils::trim(args_buffer);
        debug::log_info("Processing command: '." + command + "' with args: '" + string{trimmed_args} + "'");

        // Execute command
        if (auto it = commands_.find(command); it != commands_.end()) {
            return it->second(trimmed_args);
        }

        debug::log_warning("Unknown command: ." + command);
        return Result<void>{}; // Unknown commands are silently ignored per ROFF behavior
    }

    /**
     * @brief Process a single text character with escape sequence handling
     * @param ch Character to process
     * @return Result indicating success or failure
     */
    Result<void> process_text_character(char ch) {
        // Handle escape sequences
        ch = process_escape_sequences(ch);

        // Apply character translation
        if (ch >= 0 && ch < 128) {
            ch = translation_table_[static_cast<unsigned char>(ch)];
        }

        // Check page range constraints
        if (is_outside_page_range()) {
            return Result<void>{};
        }

        // Handle page length constraints
        if (auto result = check_page_length_limit(); !result) {
            return result;
        }

        // Process character based on type
        if (ch == '\n') {
            return process_newline();
        }

        return process_regular_character(ch);
    }

    /**
     * @brief Process escape sequences for a character
     * @param ch Input character
     * @return Processed character
     */
    char process_escape_sequences(char ch) {
        if (ch == constants::ESCAPE_CHAR) {
            if (auto next_ch = get_next_character()) {
                for (const auto& [escape_char, replacement] : escape_mappings_) {
                    if (*next_ch == escape_char) {
                        return replacement;
                    }
                }
            }
        } else if (ch == constants::PREFIX_CHAR) {
            if (auto next_ch = get_next_character()) {
                for (const auto& [prefix_char, replacement] : prefix_mappings_) {
                    if (*next_ch == prefix_char) {
                        return replacement;
                    }
                }
            }
        }
        return ch;
    }

    /**
     * @brief Check if current page is outside the specified range
     * @return True if outside range, false otherwise
     */
    bool is_outside_page_range() const noexcept {
        return page_state_.current_page < config_.start_page ||
               (config_.end_page > 0 && page_state_.current_page > config_.end_page);
    }

    /**
     * @brief Check and handle page length limits
     * @return Result indicating success or failure
     */
    Result<void> check_page_length_limit() {
        if (config_.page_length > 0 &&
            page_state_.current_line_in_page >= config_.page_length) {
            return command_break_page("");
        }
        return Result<void>{};
    }

    /**
     * @brief Process a newline character
     * @return Result indicating success or failure
     */
    Result<void> process_newline() {
        line_buffer_.push_back('\n');
        auto result = format_and_output_line(true);
        line_buffer_.clear();
        return result;
    }

    /**
     * @brief Process a regular text character
     * @param ch Character to process
     * @return Result indicating success or failure
     */
    Result<void> process_regular_character(char ch) {
        line_buffer_.push_back(ch);

        if (config_.fill_mode && should_wrap_line()) {
            return handle_line_wrap();
        }

        return Result<void>{};
    }

    /**
     * @brief Check if current line should be wrapped
     * @return True if line should wrap, false otherwise
     */
    bool should_wrap_line() const noexcept {
        return config_.line_length > 0 &&
               calculate_display_width(line_buffer_) >= config_.line_length;
    }

    /**
     * @brief Handle line wrapping in fill mode
     * @return Result indicating success or failure
     */
    Result<void> handle_line_wrap() {
        auto [part_to_output, remainder] = find_word_break(line_buffer_, config_.line_length);

        line_buffer_ = part_to_output;
        auto result = format_and_output_line(true);
        line_buffer_ = remainder;

        return result;
    }

    /**
     * @brief Find optimal word break position for line wrapping
     * @param current_line Line to analyze
     * @param max_length Maximum line length
     * @return Pair of [line_part, remainder]
     */
    std::pair<string, string> find_word_break(const string& current_line, int max_length) const {
        if (static_cast<int>(current_line.length()) <= max_length) {
            return {current_line, {}};
        }

        // Find last space within length limit
        const auto break_pos = current_line.rfind(' ', static_cast<std::size_t>(max_length));

        if (break_pos == string::npos || break_pos == 0) {
            // No space found - break mid-word
            return {current_line.substr(0, static_cast<std::size_t>(max_length)),
                    current_line.substr(static_cast<std::size_t>(max_length))};
        }

        auto remainder = current_line.substr(break_pos + 1);
        return {current_line.substr(0, break_pos), remainder};
    }

    /**
     * @brief Format and output the current line with proper alignment
     * @param add_newline Whether to add a newline character
     * @return Result indicating success or failure
     */
    Result<void> format_and_output_line(bool add_newline) {
        if (line_buffer_.empty() && !add_newline) {
            return Result<void>{};
        }

        auto processed_line = prepare_line_for_formatting();
        processed_line = apply_formatting(processed_line);

        // Add to output buffer
        for (char c : processed_line) {
            if (!output_buffer_.append(c)) {
                if (auto result = flush_output_buffer(); !result) {
                    return result;
                }
                if (!output_buffer_.append(c)) {
                    return Result<void>{ErrorCode::BufferOverflow};
                }
            }
        }

        // Handle newline and state updates
        return finalize_line_output(add_newline);
    }

    /**
     * @brief Prepare line content for formatting
     * @return Processed line content
     */
    string prepare_line_for_formatting() {
        string processed_line = line_buffer_;

        // Remove trailing newline for formatting
        if (!processed_line.empty() && processed_line.back() == '\n') {
            processed_line.pop_back();
        }

        return processed_line;
    }

    /**
     * @brief Apply formatting (indentation, centering, justification)
     * @param line Line to format
     * @return Formatted line
     */
    string apply_formatting(const string& line) {
        auto formatted_line = apply_indentation(line);

        if (config_.centering_lines_count > 0) {
            formatted_line = apply_centering(formatted_line, get_effective_line_length());
        } else if (config_.fill_mode && config_.adjust_mode != TextAlignment::Left) {
            formatted_line = apply_justification(formatted_line, get_effective_line_length());
        }

        return formatted_line;
    }

    /**
     * @brief Apply indentation to a line
     * @param line Line to indent
     * @return Indented line
     */
    string apply_indentation(const string& line) const {
        const int indent_value = config_.apply_temp_indent_once ? config_.temp_indent : config_.indent;

        if (indent_value <= 0) {
            return line;
        }

        return string(static_cast<std::size_t>(indent_value), ' ') + line;
    }

    /**
     * @brief Apply text centering
     * @param text Text to center
     * @param target_width Target line width
     * @return Centered text
     */
    string apply_centering(const string& text, int target_width) const {
        const int text_width = calculate_display_width(text);
        const int padding = std::max(0, (target_width - text_width) / 2);

        return string(static_cast<std::size_t>(padding), ' ') + text;
    }

    /**
     * @brief Apply text justification
     * @param text Text to justify
     * @param target_width Target line width
     * @return Justified text
     */
    string apply_justification(const string& text, int target_width) const {
        // Remove trailing spaces
        auto justified_text = text;
        while (!justified_text.empty() && justified_text.back() == ' ') {
            justified_text.pop_back();
        }

        switch (config_.adjust_mode) {
        case TextAlignment::Right: {
            const int text_width = calculate_display_width(justified_text);
            const int padding = std::max(0, target_width - text_width);
            return string(static_cast<std::size_t>(padding), ' ') + justified_text;
        }
        case TextAlignment::Center:
            return apply_centering(justified_text, target_width);
        case TextAlignment::Both:
            // Full justification would require space distribution
            [[fallthrough]];
        case TextAlignment::Left:
        default:
            return justified_text;
        }
    }

    /**
     * @brief Finalize line output and update state
     * @param add_newline Whether to add newline character
     * @return Result indicating success or failure
     */
    Result<void> finalize_line_output(bool add_newline) {
        if (add_newline) {
            if (!output_buffer_.append('\n')) {
                if (auto result = flush_output_buffer(); !result) {
                    return result;
                }
                if (!output_buffer_.append('\n')) {
                    return Result<void>{ErrorCode::BufferOverflow};
                }
            }

            ++page_state_.current_line_in_page;

            // Update state flags
            if (config_.apply_temp_indent_once) {
                config_.apply_temp_indent_once = false;
            }
            if (config_.centering_lines_count > 0) {
                --config_.centering_lines_count;
            }
        }

        return Result<void>{};
    }

    /**
     * @brief Calculate display width of text
     * @param text Text to measure
     * @return Display width in character positions
     */
    int calculate_display_width(const string& text) const noexcept {
        // Simple implementation - could be enhanced for tabs, Unicode, etc.
        return static_cast<int>(text.length());
    }

    /**
     * @brief Get effective line length for formatting
     * @return Effective line length
     */
    int get_effective_line_length() const noexcept {
        return config_.line_length > 0 ? config_.line_length : constants::DEFAULT_LINE_LENGTH;
    }

    /**
     * @brief Flush output buffer to stdout
     * @return Result indicating success or failure
     */
    Result<void> flush_output_buffer() {
        if (!output_buffer_.empty()) {
            const auto data = output_buffer_.used_space();
            std::cout.write(data.data(), static_cast<std::streamsize>(data.size()));
            output_buffer_.clear();

            if (std::cout.fail()) {
                return Result<void>{ErrorCode::OutputError};
            }
        }
        return Result<void>{};
    }

    // Command implementation methods

    /**
     * @brief Break current line (.br command)
     * @return Result indicating success or failure
     */
    Result<void> command_break_line() {
        auto result = format_and_output_line(true);
        line_buffer_.clear();
        return result;
    }

    /**
     * @brief Break to new page (.bp command)
     * @param args Optional page number
     * @return Result indicating success or failure
     */
    Result<void> command_break_page(string_view args) {
        // Flush current line
        if (!line_buffer_.empty()) {
            if (auto result = format_and_output_line(true); !result) {
                return result;
            }
            line_buffer_.clear();
        }

        // Flush output buffer before form feed
        if (auto result = flush_output_buffer(); !result) {
            return result;
        }

        // Handle optional page number
        if (!args.empty()) {
            if (auto page_num = parse_utils::parse_int(args)) {
                page_state_.current_page = *page_num;
            }
        } else {
            ++page_state_.current_page;
        }

        page_state_.current_line_in_page = 0;

        // Output form feed
        if (!output_buffer_.append('\f')) {
            return Result<void>{ErrorCode::BufferOverflow};
        }

        return Result<void>{};
    }

    /**
     * @brief Add spacing lines (.sp command)
     * @param args Number of lines to space
     * @return Result indicating success or failure
     */
    Result<void> command_space_lines(string_view args) {
        // Flush current line first
        if (!line_buffer_.empty()) {
            if (auto result = format_and_output_line(true); !result) {
                return result;
            }
            line_buffer_.clear();
        }

        int lines = 1;
        if (!args.empty()) {
            if (auto parsed = parse_utils::parse_int(args)) {
                lines = std::max(0, *parsed);
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        // Output the requested number of blank lines
        for (int i = 0; i < lines; ++i) {
            if (auto result = format_and_output_line(true); !result) {
                return result;
            }
        }

        return Result<void>{};
    }

    /**
     * @brief Center specified number of lines (.ce command)
     * @param args Number of lines to center
     * @return Result indicating success or failure
     */
    Result<void> command_center_lines(string_view args) {
        int lines = 1;
        if (!args.empty()) {
            if (auto parsed = parse_utils::parse_int(args)) {
                lines = std::max(0, *parsed);
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        // Flush current line normally before starting centering
        if (!line_buffer_.empty()) {
            if (auto result = format_and_output_line(true); !result) {
                return result;
            }
            line_buffer_.clear();
        }

        config_.centering_lines_count = lines;
        debug::log_info("Centering " + std::to_string(lines) + " lines");

        return Result<void>{};
    }

    /**
     * @brief Set text indentation (.in command)
     * @param args Indentation specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_indent(string_view args) {
        if (args.empty()) {
            config_.indent = config_.previous_indent.value_or(0);
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.previous_indent = config_.indent;
                config_.indent += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.previous_indent = config_.indent;
                config_.indent = *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        config_.indent = std::max(0, config_.indent);
        debug::log_info("Indent set to " + std::to_string(config_.indent));

        return Result<void>{};
    }

    /**
     * @brief Set line length (.ll command)
     * @param args Line length specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_line_length(string_view args) {
        if (args.empty()) {
            config_.line_length = constants::DEFAULT_LINE_LENGTH;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.line_length += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.line_length = *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        config_.line_length = std::max(10, config_.line_length);
        debug::log_info("Line length set to " + std::to_string(config_.line_length));

        return Result<void>{};
    }

    /**
     * @brief Set temporary indentation (.ti command)
     * @param args Temporary indent specification
     * @return Result indicating success or failure
     */
    Result<void> command_temporary_indent(string_view args) {
        if (args.empty()) {
            config_.temp_indent = 0;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.temp_indent = config_.indent + *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.temp_indent = *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        config_.temp_indent = std::max(0, config_.temp_indent);
        config_.apply_temp_indent_once = true;
        debug::log_info("Temporary indent set to " + std::to_string(config_.temp_indent));

        return Result<void>{};
    }

    /**
     * @brief Set page length (.pl command)
     * @param args Page length specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_page_length(string_view args) {
        if (args.empty()) {
            config_.page_length = constants::DEFAULT_PAGE_LENGTH;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.page_length += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.page_length = *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        config_.page_length = std::max(0, config_.page_length);
        debug::log_info("Page length set to " + std::to_string(config_.page_length));

        return Result<void>{};
    }

    /**
     * @brief Set text adjustment mode (.ad command)
     * @param args Adjustment mode specification
     * @return Result indicating success or failure
     */
    Result<void> command_adjust_text(string_view args) {
        if (args.empty() || args == "b" || args == "B") {
            config_.adjust_mode = TextAlignment::Both;
        } else if (args == "l" || args == "L") {
            config_.adjust_mode = TextAlignment::Left;
        } else if (args == "r" || args == "R") {
            config_.adjust_mode = TextAlignment::Right;
        } else if (args == "c" || args == "C") {
            config_.adjust_mode = TextAlignment::Center;
        } else {
            return Result<void>{ErrorCode::InvalidArgument};
        }

        debug::log_info("Adjust mode set to " + std::to_string(static_cast<int>(config_.adjust_mode)));
        return Result<void>{};
    }

    /**
     * @brief Source external file (.so command)
     * @param args Filename to source
     * @return Result indicating success or failure
     */
    Result<void> command_source_file(string_view args) {
        if (args.empty()) {
            return Result<void>{ErrorCode::InvalidArgument};
        }

        debug::log_info("Sourcing file: " + string{args});
        return add_input_file(args, true);
    }

    /**
     * @brief Switch to next file (.nx command)
     * @param args Filename to switch to
     * @return Result indicating success or failure
     */
    Result<void> command_next_file(string_view args) {
        if (args.empty()) {
            return Result<void>{ErrorCode::InvalidArgument};
        }

        debug::log_info("Switching to file: " + string{args});

        // Close current file
        if (current_file_index_ < input_files_.size() &&
            input_files_[current_file_index_] &&
            input_files_[current_file_index_]->is_open()) {
            input_files_[current_file_index_]->close();
        }

        // Clear all files and add the new one
        input_files_.clear();
        current_file_index_ = 0;

        return add_input_file(args);
    }
};

// Static member definitions
const std::array<std::pair<char, char>, 10> RoffProcessor::escape_mappings_ = {{
    {'d', '\032'}, {'u', '\035'}, {'r', '\036'},
    {'x', '\016'}, {'y', '\017'}, {'l', '\177'},
    {'t', '\t'}, {'a', '@'}, {'n', '#'}, {'\\', '\\'}
}};

const std::array<std::pair<char, char>, 7> RoffProcessor::prefix_mappings_ = {{
    {'7', '\036'}, {'8', '\035'}, {'9', '\032'},
    {'4', '\b'}, {'3', '\r'}, {'1', '\026'}, {'2', '\027'}
}};

} // namespace engine
} // namespace roff

/**
 * @brief Modern C++ main function with comprehensive error handling
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char* argv[]) try {
    using namespace roff::engine;

    // Convert C-style arguments to modern C++ containers
    std::vector<std::string_view> args;
    args.reserve(static_cast<std::size_t>(std::max(0, argc - 1)));

    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    // Create processor with default configuration
    RoffProcessor processor;

    // Process command line arguments
    if (auto result = processor.process_arguments(args); !result) {
        roff::debug::log_error("Error processing arguments: " + std::to_string(static_cast<int>(result.error())));
        return 1;
    }

    // Execute main processing
    if (auto result = processor.process(); !result) {
        // Attempt to flush any remaining content before exiting
        [[maybe_unused]] auto flush_result = processor.flush_final_content();

        roff::debug::log_error("Error during processing: " + std::to_string(static_cast<int>(result.error())));
        return 1;
    }

    // Ensure all content is properly flushed
    if (auto flush_result = processor.flush_final_content(); !flush_result) {
        roff::debug::log_error("Error flushing final content: " + std::to_string(static_cast<int>(flush_result.error())));
        return 1;
    }

    return 0;

} catch (const roff::RoffException& e) {
    roff::debug::log_error("ROFF error [" + std::to_string(static_cast<int>(e.code())) + "]: " + e.what() +
                          " at " + e.location().file_name() + ":" + std::to_string(e.location().line()));
    return 2;
} catch (const std::exception& e) {
    roff::debug::log_error("Fatal error: " + std::string{e.what()});
    return 2;
} catch (...) {
    roff::debug::log_error("Unknown fatal error occurred");
    return 3;
}
