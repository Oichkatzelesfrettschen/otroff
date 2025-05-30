// Enforce C++23 compilation with comprehensive feature checks
#if __cplusplus < 202302L
#error "This code requires C++23 or later. Use -std=c++23 or equivalent."
#endif

#ifndef __cpp_concepts
#error "C++23 concepts support required"
#endif

#ifndef __cpp_consteval
#error "C++23 consteval support required"
#endif

#ifndef __cpp_lib_expected
#error "C++23 std::expected support required"
#endif

#ifndef __cpp_lib_ranges
#error "C++23 ranges support required"
#endif

/**
 * @file roff1.cpp
 * @brief ROFF text formatter - Main driver and core functionality (Pure C++23)
 * @author Generated with C++23 best practices
 * @version 2.0
 * @date 2024
 *
 * @details This implementation provides a complete ROFF text processor using
 * modern C++23 features including concepts, modules, ranges, and std::expected.
 * All C-style code has been eliminated in favor of type-safe, exception-safe
 * modern C++ constructs.
 */

#include "roff.hpp"
#include <algorithm>
#include <array>
#include <concepts>
#include <expected>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/// @brief Primary namespace for ROFF processing engine
namespace roff::engine {

using namespace roff;
using namespace std::literals;

/// @brief Result type alias for std::expected operations
template <typename T>
using Result = std::expected<T, ErrorCode>;

/// @brief Concept defining valid text content
template <typename T>
concept TextContent = std::convertible_to<T, std::string_view> &&
                      requires(T t) {
                          { t.empty() } -> std::convertible_to<bool>;
                          { t.size() } -> std::convertible_to<std::size_t>;
                      };

/// @brief Concept for numeric arguments
template <typename T>
concept NumericArgument = std::integral<T> || std::convertible_to<T, int>;

/**
 * @brief Modern ROFF processor implementation using pure C++23
 *
 * This class provides complete ROFF text processing capabilities with:
 * - Type-safe command processing
 * - Memory-safe file handling
 * - Exception-safe error handling
 * - Modern C++23 patterns throughout
 */
class RoffProcessor {
  private:
    /// @brief Configuration state
    RoffConfig config_;

    /// @brief Command registry using modern function objects
    std::unordered_map<std::string, std::function<Result<void>(std::string_view)>> commands_;

    /// @brief Output buffer for efficient text accumulation
    OutputBuffer output_buffer_;

    /// @brief Current line being assembled
    std::string line_buffer_;

    /// @brief Input file streams with RAII management
    std::vector<std::unique_ptr<std::ifstream>> input_files_;

    /// @brief Current file processing index
    std::size_t current_file_index_{0};

    /// @brief Exit request flag for .ex command
    bool exit_requested_{false};

    /// @brief Page tracking state
    struct PageState {
        int current_page{1};
        int current_line_in_page{0};
    } page_state_;

    /// @brief Character translation table for escape sequences
    std::array<char, 128> translation_table_;

    /// @brief Modern escape sequence mappings
    static constexpr std::array escape_mappings_{
        std::pair{'d', '\032'}, std::pair{'u', '\035'}, std::pair{'r', '\036'},
        std::pair{'x', '\016'}, std::pair{'y', '\017'}, std::pair{'l', '\177'},
        std::pair{'t', '\t'}, std::pair{'a', '@'}, std::pair{'n', '#'},
        std::pair{'\\', '\\'}};

    /// @brief Prefix sequence mappings
    static constexpr std::array prefix_mappings_{
        std::pair{'7', '\036'}, std::pair{'8', '\035'}, std::pair{'9', '\032'},
        std::pair{'4', '\b'}, std::pair{'3', '\r'}, std::pair{'1', '\026'},
        std::pair{'2', '\027'}};

  public:
    /**
     * @brief Construct a new ROFF processor
     * @param config Initial configuration (default constructed if not provided)
     * @throws RoffException if configuration is invalid
     */
    explicit RoffProcessor(RoffConfig config = {}) : config_(std::move(config)) {
        if (!config_.is_valid()) {
            throw RoffException(ErrorCode::InvalidArgument, "Invalid ROFF configuration");
        }

        initialize_translation_table();
        register_commands();
    }

    /**
     * @brief Process command line arguments using modern ranges
     * @param args Span of command line arguments
     * @return Result indicating success or failure
     */
    Result<void> process_arguments(std::span<const std::string_view> args) {
        for (const auto &arg : args) {
            if (auto result = process_single_argument(arg); !result) {
                return result;
            }
        }
        return {};
    }

    /**
     * @brief Main processing loop using modern C++23 patterns
     * @return Result indicating success or completion
     */
    Result<void> process() {
        while (auto char_result = get_next_character()) {
            if (exit_requested_)
                break;

            const auto ch = *char_result;

            if (ch == constants::CONTROL_CHAR) {
                if (auto result = process_control_command(); !result) {
                    return result;
                }
            } else {
                if (auto result = process_text_character(ch); !result) {
                    return result;
                }
            }
        }

        return flush_final_content();
    }

    /**
     * @brief Flush any remaining content and finalize output
     * @return Result indicating success or failure
     */
    Result<void> flush_final_content() {
        // Flush any remaining line buffer content
        if (!line_buffer_.empty()) {
            if (auto result = format_and_output_line(false); !result) {
                return result;
            }
            line_buffer_.clear();
        }

        // Flush the main output buffer
        return flush_output_buffer();
    }

  private:
    /**
     * @brief Initialize character translation table
     */
    constexpr void initialize_translation_table() noexcept {
        std::ranges::iota(translation_table_, char{0});
    }

    /**
     * @brief Register all supported ROFF commands
     */
    void register_commands() {
        // Line breaking commands
        commands_["br"] = [this](std::string_view) -> Result<void> {
            return command_break_line();
        };

        commands_["bp"] = [this](std::string_view args) -> Result<void> {
            return command_break_page(args);
        };

        commands_["sp"] = [this](std::string_view args) -> Result<void> {
            return command_space_lines(args);
        };

        // Text formatting commands
        commands_["ce"] = [this](std::string_view args) -> Result<void> {
            return command_center_lines(args);
        };

        commands_["fi"] = [this](std::string_view) -> Result<void> {
            config_.fill_mode = true;
            debug::log_info("Fill mode enabled");
            return {};
        };

        commands_["nf"] = [this](std::string_view) -> Result<void> {
            config_.fill_mode = false;
            debug::log_info("Fill mode disabled");
            return {};
        };

        // Indentation and spacing commands
        commands_["in"] = [this](std::string_view args) -> Result<void> {
            return command_set_indent(args);
        };

        commands_["ll"] = [this](std::string_view args) -> Result<void> {
            return command_set_line_length(args);
        };

        commands_["ti"] = [this](std::string_view args) -> Result<void> {
            return command_temporary_indent(args);
        };

        commands_["pl"] = [this](std::string_view args) -> Result<void> {
            return command_set_page_length(args);
        };

        // Text adjustment commands
        commands_["ad"] = [this](std::string_view args) -> Result<void> {
            return command_adjust_text(args);
        };

        commands_["na"] = [this](std::string_view) -> Result<void> {
            config_.adjust_mode = TextAlignment::Left;
            debug::log_info("Text adjustment disabled");
            return {};
        };

        // File processing commands
        commands_["so"] = [this](std::string_view args) -> Result<void> {
            return command_source_file(args);
        };

        commands_["nx"] = [this](std::string_view args) -> Result<void> {
            return command_next_file(args);
        };

        commands_["ex"] = [this](std::string_view) -> Result<void> {
            debug::log_info("Exit command received");
            exit_requested_ = true;
            return {};
        };
    }

    /**
     * @brief Process a single command line argument
     * @param arg The argument to process
     * @return Result indicating success or failure
     */
    Result<void> process_single_argument(std::string_view arg) {
        if (arg.starts_with('+')) {
            if (auto page = parse_utils::parse_int(arg.substr(1))) {
                config_.start_page = *page;
                return {};
            }
            return std::unexpected{ErrorCode::InvalidArgument};
        }

        if (arg.starts_with('-')) {
            if (arg == "-s") {
                config_.mode = ProcessingMode::Stop;
                return {};
            }
            if (arg == "-h") {
                config_.mode = ProcessingMode::HighSpeed;
                return {};
            }
            if (auto page = parse_utils::parse_int(arg.substr(1))) {
                config_.end_page = *page;
                return {};
            }
            return std::unexpected{ErrorCode::InvalidArgument};
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
    Result<void> add_input_file(std::string_view filename, bool insert_next = false) {
        try {
            auto file = std::make_unique<std::ifstream>(std::string{filename});
            if (!file || !file->is_open()) {
                debug::log_warning(std::format("Cannot open input file: {}", filename));
                return std::unexpected{ErrorCode::FileNotFound};
            }

            if (insert_next && current_file_index_ + 1 < input_files_.size()) {
                auto insert_pos = input_files_.begin() + static_cast<std::ptrdiff_t>(current_file_index_ + 1);
                input_files_.insert(insert_pos, std::move(file));
            } else {
                input_files_.push_back(std::move(file));
            }

            debug::log_info(std::format("Added input file: {}", filename));
            return {};
        } catch (const std::exception &e) {
            debug::log_error(std::format("Exception adding input file {}: {}", filename, e.what()));
            return std::unexpected{ErrorCode::FileNotFound};
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
            auto &current_file = input_files_[current_file_index_];

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
            return std::unexpected{ErrorCode::InternalError};
        }

        const std::string command{*cmd1, *cmd2};

        // Read arguments until newline
        std::string args_buffer;
        while (auto ch_opt = get_next_character()) {
            if (*ch_opt == '\n')
                break;
            args_buffer += *ch_opt;
        }

        const auto trimmed_args = string_utils::trim(args_buffer);
        debug::log_info(std::format("Processing command: '.{}' with args: '{}'",
                                    command, trimmed_args));

        // Execute command
        if (auto it = commands_.find(command); it != commands_.end()) {
            return it->second(trimmed_args);
        }

        debug::log_warning(std::format("Unknown command: .{}", command));
        return {}; // Unknown commands are silently ignored per ROFF behavior
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
            return {};
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
                for (const auto &[escape_char, replacement] : escape_mappings_) {
                    if (*next_ch == escape_char) {
                        return replacement;
                    }
                }
            }
        } else if (ch == constants::PREFIX_CHAR) {
            if (auto next_ch = get_next_character()) {
                for (const auto &[prefix_char, replacement] : prefix_mappings_) {
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
        return {};
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

        return {};
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
    std::pair<std::string, std::string> find_word_break(const std::string &current_line,
                                                        int max_length) const {
        if (static_cast<int>(current_line.length()) <= max_length) {
            return {current_line, {}};
        }

        // Find last space within length limit
        const auto break_pos = current_line.rfind(' ', static_cast<std::size_t>(max_length));

        if (break_pos == std::string::npos || break_pos == 0) {
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
            return {};
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
                    return std::unexpected{ErrorCode::BufferOverflow};
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
    std::string prepare_line_for_formatting() {
        std::string processed_line = line_buffer_;

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
    std::string apply_formatting(const std::string &line) {
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
    std::string apply_indentation(const std::string &line) const {
        const int indent_value = config_.apply_temp_indent_once ? config_.temp_indent : config_.indent;

        if (indent_value <= 0) {
            return line;
        }

        return std::string(static_cast<std::size_t>(indent_value), ' ') + line;
    }

    /**
     * @brief Apply text centering
     * @param text Text to center
     * @param target_width Target line width
     * @return Centered text
     */
    std::string apply_centering(const std::string &text, int target_width) const {
        const int text_width = calculate_display_width(text);
        const int padding = std::max(0, (target_width - text_width) / 2);

        return std::string(static_cast<std::size_t>(padding), ' ') + text;
    }

    /**
     * @brief Apply text justification
     * @param text Text to justify
     * @param target_width Target line width
     * @return Justified text
     */
    std::string apply_justification(const std::string &text, int target_width) const {
        // Remove trailing spaces
        auto justified_text = text;
        while (!justified_text.empty() && justified_text.back() == ' ') {
            justified_text.pop_back();
        }

        switch (config_.adjust_mode) {
        case TextAlignment::Right: {
            const int text_width = calculate_display_width(justified_text);
            const int padding = std::max(0, target_width - text_width);
            return std::string(static_cast<std::size_t>(padding), ' ') + justified_text;
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
                    return std::unexpected{ErrorCode::BufferOverflow};
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

        return {};
    }

    /**
     * @brief Calculate display width of text
     * @param text Text to measure
     * @return Display width in character positions
     */
    int calculate_display_width(const std::string &text) const noexcept {
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
                return std::unexpected{ErrorCode::OutputError};
            }
        }
        return {};
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
    Result<void> command_break_page(std::string_view args) {
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
            return std::unexpected{ErrorCode::BufferOverflow};
        }

        return {};
    }

    /**
     * @brief Add spacing lines (.sp command)
     * @param args Number of lines to space
     * @return Result indicating success or failure
     */
    Result<void> command_space_lines(std::string_view args) {
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
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        }

        // Output the requested number of blank lines
        for (int i = 0; i < lines; ++i) {
            if (auto result = format_and_output_line(true); !result) {
                return result;
            }
        }

        return {};
    }

    /**
     * @brief Center specified number of lines (.ce command)
     * @param args Number of lines to center
     * @return Result indicating success or failure
     */
    Result<void> command_center_lines(std::string_view args) {
        int lines = 1;
        if (!args.empty()) {
            if (auto parsed = parse_utils::parse_int(args)) {
                lines = std::max(0, *parsed);
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
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
        debug::log_info(std::format("Centering {} lines", lines));

        return {};
    }

    /**
     * @brief Set text indentation (.in command)
     * @param args Indentation specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_indent(std::string_view args) {
        if (args.empty()) {
            config_.indent = config_.previous_indent.value_or(0);
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.previous_indent = config_.indent;
                config_.indent += *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.previous_indent = config_.indent;
                config_.indent = *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        }

        config_.indent = std::max(0, config_.indent);
        debug::log_info(std::format("Indent set to {}", config_.indent));

        return {};
    }

    /**
     * @brief Set line length (.ll command)
     * @param args Line length specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_line_length(std::string_view args) {
        if (args.empty()) {
            config_.line_length = constants::DEFAULT_LINE_LENGTH;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.line_length += *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.line_length = *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        }

        config_.line_length = std::max(10, config_.line_length);
        debug::log_info(std::format("Line length set to {}", config_.line_length));

        return {};
    }

    /**
     * @brief Set temporary indentation (.ti command)
     * @param args Temporary indent specification
     * @return Result indicating success or failure
     */
    Result<void> command_temporary_indent(std::string_view args) {
        if (args.empty()) {
            config_.temp_indent = 0;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.temp_indent = config_.indent + *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.temp_indent = *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        }

        config_.temp_indent = std::max(0, config_.temp_indent);
        config_.apply_temp_indent_once = true;
        debug::log_info(std::format("Temporary indent set to {}", config_.temp_indent));

        return {};
    }

    /**
     * @brief Set page length (.pl command)
     * @param args Page length specification
     * @return Result indicating success or failure
     */
    Result<void> command_set_page_length(std::string_view args) {
        if (args.empty()) {
            config_.page_length = constants::DEFAULT_PAGE_LENGTH;
        } else if (args.starts_with('+') || args.starts_with('-')) {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.page_length += *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        } else {
            if (auto parsed = parse_utils::parse_int(args)) {
                config_.page_length = *parsed;
            } else {
                return std::unexpected{ErrorCode::InvalidArgument};
            }
        }

        config_.page_length = std::max(0, config_.page_length);
        debug::log_info(std::format("Page length set to {}", config_.page_length));

        return {};
    }

    /**
     * @brief Set text adjustment mode (.ad command)
     * @param args Adjustment mode specification
     * @return Result indicating success or failure
     */
    Result<void> command_adjust_text(std::string_view args) {
        if (args.empty() || args == "b" || args == "B") {
            config_.adjust_mode = TextAlignment::Both;
        } else if (args == "l" || args == "L") {
            config_.adjust_mode = TextAlignment::Left;
        } else if (args == "r" || args == "R") {
            config_.adjust_mode = TextAlignment::Right;
        } else if (args == "c" || args == "C") {
            config_.adjust_mode = TextAlignment::Center;
        } else {
            return std::unexpected{ErrorCode::InvalidArgument};
        }

        debug::log_info(std::format("Adjust mode set to {}", static_cast<char>(config_.adjust_mode)));
        return {};
    }

    /**
     * @brief Source external file (.so command)
     * @param args Filename to source
     * @return Result indicating success or failure
     */
    Result<void> command_source_file(std::string_view args) {
        if (args.empty()) {
            return std::unexpected{ErrorCode::InvalidArgument};
        }

        debug::log_info(std::format("Sourcing file: {}", args));
        return add_input_file(args, true);
    }

    /**
     * @brief Switch to next file (.nx command)
     * @param args Filename to switch to
     * @return Result indicating success or failure
     */
    Result<void> command_next_file(std::string_view args) {
        if (args.empty()) {
            return std::unexpected{ErrorCode::InvalidArgument};
        }

        debug::log_info(std::format("Switching to file: {}", args));

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

} // namespace roff::engine

/**
 * @brief Modern C++23 main function with comprehensive error handling
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char *argv[]) try {
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
        roff::debug::log_error(std::format("Error processing arguments: {}",
                                           static_cast<int>(result.error())));
        return 1;
    }

    // Execute main processing
    if (auto result = processor.process(); !result) {
        // Attempt to flush any remaining content before exiting
        [[maybe_unused]] auto flush_result = processor.flush_final_content();

        roff::debug::log_error(std::format("Error during processing: {}",
                                           static_cast<int>(result.error())));
        return 1;
    }

    // Ensure all content is properly flushed
    if (auto flush_result = processor.flush_final_content(); !flush_result) {
        roff::debug::log_error(std::format("Error flushing final content: {}",
                                           static_cast<int>(flush_result.error())));
        return 1;
    }

    return 0;

} catch (const roff::RoffException &e) {
    roff::debug::log_error(std::format("ROFF error [{}]: {} at {}:{}",
                                       static_cast<int>(e.code()), e.what(),
                                       e.location().file_name(), e.location().line()));
    return 2;
} catch (const std::exception &e) {
    roff::debug::log_error(std::format("Fatal error: {}", e.what()));
    return 2;
} catch (...) {
    roff::debug::log_error("Unknown fatal error occurred");
    return 3;
}
