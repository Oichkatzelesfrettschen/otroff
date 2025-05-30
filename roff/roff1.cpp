/**
 * @file roff1.cpp
 * @brief ROFF text formatter - Simple and compatible implementation
 * @author C++ rewrite
 * @version 1.0
 * @date 2024
 *
 * @details A simple ROFF text processor compatible with C++14/17
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <array>
#include <unordered_map>
#include <functional>
#include <exception>
#include <utility>
#include <iterator>

// Simple ROFF processor compatible with C++14/17
class RoffProcessor {
  private:
    struct Config {
        bool fill_mode = true;
        int indent = 0;
        int temp_indent = 0;
        int line_length = 65;
        int page_length = 66;
        int start_page = 1;
        int end_page = 0;
        int centering_lines_count = 0;
        bool apply_temp_indent_once = false;
        int previous_indent = 0;
        int adjust_mode = 0; // 0=left, 1=right, 2=center, 3=both
    };

    Config config_;
    std::string line_buffer_;
    std::vector<std::unique_ptr<std::ifstream>> input_files_;
    size_t current_file_index_ = 0;
    bool exit_requested_ = false;
    int current_page_ = 1;
    int current_line_in_page_ = 0;

  public:
    RoffProcessor() = default;

    bool process_arguments(const std::vector<std::string> &args) {
        for (const auto &arg : args) {
            if (arg.empty())
                continue;

            if (arg[0] == '+') {
                config_.start_page = std::atoi(arg.c_str() + 1);
            } else if (arg[0] == '-') {
                if (arg == "-s") {
                    // Stop mode - not implemented
                } else if (arg == "-h") {
                    // High speed mode - not implemented
                } else {
                    config_.end_page = std::atoi(arg.c_str() + 1);
                }
            } else {
                // Input file
                auto file = std::make_unique<std::ifstream>(arg);
                if (!file->is_open()) {
                    std::cerr << "Cannot open file: " << arg << std::endl;
                    return false;
                }
                input_files_.push_back(std::move(file));
            }
        }

        // If no files, read from stdin
        if (input_files_.empty()) {
            // We'll handle stdin in the main loop
        }

        return true;
    }

    bool process() {
        char ch;
        while (get_next_character(ch)) {
            if (exit_requested_)
                break;

            if (ch == '.') {
                if (!process_control_command()) {
                    return false;
                }
            } else {
                process_text_character(ch);
            }
        }

        flush_final_content();
        return true;
    }

  private:
    bool get_next_character(char &ch) {
        if (exit_requested_)
            return false;

        // Try reading from current file
        while (current_file_index_ < input_files_.size()) {
            auto &current_file = input_files_[current_file_index_];

            if (current_file && current_file->is_open() && current_file->get(ch)) {
                return true;
            }

            // Move to next file
            ++current_file_index_;
        }

        // If no files or all files exhausted, try stdin
        if (input_files_.empty() || current_file_index_ >= input_files_.size()) {
            if (std::cin.get(ch)) {
                return true;
            }
        }

        return false;
    }

    bool process_control_command() {
        char cmd1, cmd2;
        if (!get_next_character(cmd1) || !get_next_character(cmd2)) {
            return false;
        }

        std::string command;
        command += cmd1;
        command += cmd2;

        // Read arguments until newline
        std::string args;
        char ch;
        while (get_next_character(ch) && ch != '\n') {
            args += ch;
        }

        // Trim whitespace
        args.erase(0, args.find_first_not_of(" \t"));
        args.erase(args.find_last_not_of(" \t") + 1);

        return execute_command(command, args);
    }

    bool execute_command(const std::string &command, const std::string &args) {
        if (command == "br") {
            return command_break_line();
        } else if (command == "bp") {
            return command_break_page(args);
        } else if (command == "sp") {
            return command_space_lines(args);
        } else if (command == "ce") {
            return command_center_lines(args);
        } else if (command == "fi") {
            config_.fill_mode = true;
            return true;
        } else if (command == "nf") {
            config_.fill_mode = false;
            return true;
        } else if (command == "in") {
            return command_set_indent(args);
        } else if (command == "ll") {
            return command_set_line_length(args);
        } else if (command == "ti") {
            return command_temporary_indent(args);
        } else if (command == "pl") {
            return command_set_page_length(args);
        } else if (command == "ad") {
            return command_adjust_text(args);
        } else if (command == "na") {
            config_.adjust_mode = 0; // left
            return true;
        } else if (command == "so") {
            return command_source_file(args);
        } else if (command == "nx") {
            return command_next_file(args);
        } else if (command == "ex") {
            exit_requested_ = true;
            return true;
        }

        // Unknown command - silently ignore
        return true;
    }

    void process_text_character(char ch) {
        // Handle escape sequences
        ch = process_escape_sequences(ch);

        // Check page range
        if (is_outside_page_range()) {
            return;
        }

        // Handle page length
        if (config_.page_length > 0 && current_line_in_page_ >= config_.page_length) {
            command_break_page("");
        }

        if (ch == '\n') {
            process_newline();
        } else {
            line_buffer_ += ch;

            if (config_.fill_mode && should_wrap_line()) {
                handle_line_wrap();
            }
        }
    }

    char process_escape_sequences(char ch) {
        // Simple escape sequence handling
        if (ch == '\\') {
            char next;
            if (get_next_character(next)) {
                switch (next) {
                case 't':
                    return '\t';
                case 'n':
                    return '\n';
                case '\\':
                    return '\\';
                default:
                    return next;
                }
            }
        }
        return ch;
    }

    bool is_outside_page_range() const {
        return current_page_ < config_.start_page ||
               (config_.end_page > 0 && current_page_ > config_.end_page);
    }

    void process_newline() {
        output_line();
        line_buffer_.clear();
    }

    bool should_wrap_line() const {
        return config_.line_length > 0 &&
               static_cast<int>(line_buffer_.length()) >= config_.line_length;
    }

    void handle_line_wrap() {
        // Find last space for word wrapping
        size_t break_pos = line_buffer_.rfind(' ', config_.line_length);

        if (break_pos == std::string::npos || break_pos == 0) {
            // No space found - break at line length
            break_pos = config_.line_length;
        }

        std::string part_to_output = line_buffer_.substr(0, break_pos);
        std::string remainder = line_buffer_.substr(break_pos + 1);

        line_buffer_ = part_to_output;
        output_line();
        line_buffer_ = remainder;
    }

    void output_line() {
        if (line_buffer_.empty()) {
            std::cout << '\n';
            ++current_line_in_page_;
            return;
        }

        std::string formatted_line = format_line(line_buffer_);
        std::cout << formatted_line << '\n';
        ++current_line_in_page_;

        // Update state
        if (config_.apply_temp_indent_once) {
            config_.apply_temp_indent_once = false;
        }
        if (config_.centering_lines_count > 0) {
            --config_.centering_lines_count;
        }
    }

    std::string format_line(const std::string &line) {
        std::string result = line;

        // Apply indentation
        int indent_value = config_.apply_temp_indent_once ? config_.temp_indent : config_.indent;

        if (indent_value > 0) {
            result = std::string(indent_value, ' ') + result;
        }

        // Apply centering
        if (config_.centering_lines_count > 0) {
            int padding = std::max(0, (config_.line_length - static_cast<int>(result.length())) / 2);
            result = std::string(padding, ' ') + result;
        }

        return result;
    }

    void flush_final_content() {
        if (!line_buffer_.empty()) {
            output_line();
        }
    }

    // Command implementations
    bool command_break_line() {
        output_line();
        line_buffer_.clear();
        return true;
    }

    bool command_break_page(const std::string &args) {
        if (!line_buffer_.empty()) {
            output_line();
            line_buffer_.clear();
        }

        if (!args.empty()) {
            current_page_ = std::atoi(args.c_str());
        } else {
            ++current_page_;
        }

        current_line_in_page_ = 0;
        std::cout << '\f'; // Form feed
        return true;
    }

    bool command_space_lines(const std::string &args) {
        if (!line_buffer_.empty()) {
            output_line();
            line_buffer_.clear();
        }

        int lines = args.empty() ? 1 : std::atoi(args.c_str());
        for (int i = 0; i < lines; ++i) {
            std::cout << '\n';
            ++current_line_in_page_;
        }
        return true;
    }

    bool command_center_lines(const std::string &args) {
        if (!line_buffer_.empty()) {
            output_line();
            line_buffer_.clear();
        }

        config_.centering_lines_count = args.empty() ? 1 : std::atoi(args.c_str());
        return true;
    }

    bool command_set_indent(const std::string &args) {
        if (args.empty()) {
            config_.indent = config_.previous_indent;
        } else if (args[0] == '+') {
            config_.previous_indent = config_.indent;
            config_.indent += std::atoi(args.c_str() + 1);
        } else if (args[0] == '-') {
            config_.previous_indent = config_.indent;
            config_.indent -= std::atoi(args.c_str() + 1);
        } else {
            config_.previous_indent = config_.indent;
            config_.indent = std::atoi(args.c_str());
        }

        config_.indent = std::max(0, config_.indent);
        return true;
    }

    bool command_set_line_length(const std::string &args) {
        if (args.empty()) {
            config_.line_length = 65;
        } else if (args[0] == '+') {
            config_.line_length += std::atoi(args.c_str() + 1);
        } else if (args[0] == '-') {
            config_.line_length -= std::atoi(args.c_str() + 1);
        } else {
            config_.line_length = std::atoi(args.c_str());
        }

        config_.line_length = std::max(10, config_.line_length);
        return true;
    }

    bool command_temporary_indent(const std::string &args) {
        if (args.empty()) {
            config_.temp_indent = 0;
        } else if (args[0] == '+') {
            config_.temp_indent = config_.indent + std::atoi(args.c_str() + 1);
        } else if (args[0] == '-') {
            config_.temp_indent = config_.indent - std::atoi(args.c_str() + 1);
        } else {
            config_.temp_indent = std::atoi(args.c_str());
        }

        config_.temp_indent = std::max(0, config_.temp_indent);
        config_.apply_temp_indent_once = true;
        return true;
    }

    bool command_set_page_length(const std::string &args) {
        if (args.empty()) {
            config_.page_length = 66;
        } else if (args[0] == '+') {
            config_.page_length += std::atoi(args.c_str() + 1);
        } else if (args[0] == '-') {
            config_.page_length -= std::atoi(args.c_str() + 1);
        } else {
            config_.page_length = std::atoi(args.c_str());
        }

        config_.page_length = std::max(0, config_.page_length);
        return true;
    }

    bool command_adjust_text(const std::string &args) {
        if (args.empty() || args == "b" || args == "B") {
            config_.adjust_mode = 3; // both
        } else if (args == "l" || args == "L") {
            config_.adjust_mode = 0; // left
        } else if (args == "r" || args == "R") {
            config_.adjust_mode = 1; // right
        } else if (args == "c" || args == "C") {
            config_.adjust_mode = 2; // center
        }
        return true;
    }

    bool command_source_file(const std::string &args) {
        if (args.empty())
            return false;

        auto file = std::make_unique<std::ifstream>(args);
        if (!file->is_open()) {
            std::cerr << "Cannot open file: " << args << std::endl;
            return false;
        }

        // Insert after current file
        auto insert_pos = input_files_.begin() + current_file_index_ + 1;
        input_files_.insert(insert_pos, std::move(file));
        return true;
    }

    bool command_next_file(const std::string &args) {
        if (args.empty())
            return false;

        // Close current files and switch to new one
        input_files_.clear();
        current_file_index_ = 0;

        auto file = std::make_unique<std::ifstream>(args);
        if (!file->is_open()) {
            std::cerr << "Cannot open file: " << args << std::endl;
            return false;
        }

        input_files_.push_back(std::move(file));
        return true;
    }
};

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
    bool fill_mode;
    int indent;
    int temp_indent;
    int line_length;
    int page_length;
    int start_page;
    int end_page;
    int centering_lines_count;
    bool apply_temp_indent_once;
    optional<int> previous_indent;
    TextAlignment adjust_mode;
    ProcessingMode mode;

    RoffConfig()
        : fill_mode(true), indent(0), temp_indent(0), line_length(65), page_length(66), start_page(1), end_page(0), centering_lines_count(0), apply_temp_indent_once(false), adjust_mode(TextAlignment::Left), mode(ProcessingMode::Normal) {}

    bool is_valid() const {
        return line_length > 0 && page_length >= 0 && start_page > 0;
    }
};

class RoffException : public std::exception {
  private:
    ErrorCode code_;
    std::string message_;

  public:
    RoffException(ErrorCode code, const std::string &message)
        : code_(code), message_(message) {}

    const char *what() const noexcept override { return message_.c_str(); }
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
} // namespace constants

namespace debug {
void log_info(const std::string &msg) {
    std::cerr << "[INFO] " << msg << std::endl;
}
void log_warning(const std::string &msg) {
    std::cerr << "[WARN] " << msg << std::endl;
}
void log_error(const std::string &msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}
} // namespace debug

namespace string_utils {
std::string_view trim(const std::string &str) {
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return {};
    const auto end = str.find_last_not_of(" \t\r\n");
    return std::string_view(str).substr(start, end - start + 1);
}
} // namespace string_utils

namespace parse_utils {
std::optional<int> parse_int(std::string_view str) {
    try {
        std::string s(str);
        return std::stoi(s);
    } catch (...) {
        return std::nullopt;
    }
}
} // namespace parse_utils

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
        if (buffer_.size() >= capacity_)
            return false;
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
    Result(T &&val) : value_(std::move(val)) {}
    Result(const T &val) : value_(val) {}
    Result(ErrorCode err) : error_(err) {}

    bool has_value() const { return value_.has_value(); }
    operator bool() const { return has_value(); }

    T &operator*() { return *value_; }
    const T &operator*() const { return *value_; }

    T *operator->() { return &(*value_); }
    const T *operator->() const { return &(*value_); }

    ErrorCode error() const { return error_.value_or(ErrorCode::Success); }
};

template <>
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
           str.substr(0, prefix.length()) == prefix;
}

/**
 * @brief Modern ROFF processor implementation using C++17
 *
 * This class provides complete ROFF text processing capabilities with:
 * - Type-safe command processing
 * - Memory-safe file handling
 * - Exception-safe error handling
 * - Modern C++ patterns throughout
 */
class RoffProcessor {
  private:
    /// @brief Configuration state
    RoffConfig config_;

    /// @brief Command registry using modern function objects
    std::unordered_map<string, std::function<Result<void>(string_view)>> commands_;

    /// @brief Output buffer for efficient text accumulation
    OutputBuffer output_buffer_;

    /// @brief Current line being assembled
    string line_buffer_;

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
    static const std::array<std::pair<char, char>, 10> escape_mappings_;

    /// @brief Prefix sequence mappings
    static const std::array<std::pair<char, char>, 7> prefix_mappings_;

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
     * @brief Process command line arguments
     * @param args Vector of command line arguments
     * @return Result indicating success or failure
     */
    Result<void> process_arguments(const std::vector<string_view> &args) {
        for (const auto &arg : args) {
            auto result = process_single_argument(arg);
            if (!result) {
                return result;
            }
        }
        return Result<void>{};
    }

    /**
     * @brief Main processing loop using modern C++ patterns
     * @return Result indicating success or completion
     */
    Result<void> process() {
        while (auto char_result = get_next_character()) {
            if (exit_requested_)
                break;

            const auto ch = *char_result;

            if (ch == constants::CONTROL_CHAR) {
                auto result = process_control_command();
                if (!result) {
                    return result;
                }
            } else {
                auto result = process_text_character(ch);
                if (!result) {
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
            auto result = format_and_output_line(false);
            if (!result) {
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
    void initialize_translation_table() noexcept {
        for (int i = 0; i < 128; ++i) {
            translation_table_[i] = static_cast<char>(i);
        }
    }

    /**
     * @brief Register all supported ROFF commands
     */
    void register_commands() {
        // Line breaking commands
        commands_["br"] = [this](string_view) -> Result<void> {
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
        if (starts_with(arg, '+')) {
            auto page = parse_utils::parse_int(arg.substr(1));
            if (page) {
                config_.start_page = *page;
                return Result<void>{};
            }
            return Result<void>{ErrorCode::InvalidArgument};
        }

        if (starts_with(arg, '-')) {
            if (arg == "-s") {
                config_.mode = ProcessingMode::Stop;
                return Result<void>{};
            }
            if (arg == "-h") {
                config_.mode = ProcessingMode::HighSpeed;
                return Result<void>{};
            }
            auto page = parse_utils::parse_int(arg.substr(1));
            if (page) {
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
        } catch (const std::exception &e) {
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
        auto it = commands_.find(command);
        if (it != commands_.end()) {
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
        if (static_cast<unsigned char>(ch) < 128) {
            ch = translation_table_[static_cast<unsigned char>(ch)];
        }

        // Check page range constraints
        if (is_outside_page_range()) {
            return Result<void>{};
        }

        // Handle page length constraints
        auto result = check_page_length_limit();
        if (!result) {
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
            auto next_ch = get_next_character();
            if (next_ch) {
                for (const auto &pair : escape_mappings_) {
                    if (*next_ch == pair.first) {
                        return pair.second;
                    }
                }
            }
        } else if (ch == constants::PREFIX_CHAR) {
            auto next_ch = get_next_character();
            if (next_ch) {
                for (const auto &pair : prefix_mappings_) {
                    if (*next_ch == pair.first) {
                        return pair.second;
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
        auto word_break_result = find_word_break(line_buffer_, config_.line_length);
        auto part_to_output = word_break_result.first;
        auto remainder = word_break_result.second;

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
    std::pair<string, string> find_word_break(const string &current_line, int max_length) const {
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
                auto result = flush_output_buffer();
                if (!result) {
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
    string apply_formatting(const string &line) {
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
    string apply_indentation(const string &line) const {
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
    string apply_centering(const string &text, int target_width) const {
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
    string apply_justification(const string &text, int target_width) const {
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
                auto result = flush_output_buffer();
                if (!result) {
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
    int calculate_display_width(const string &text) const noexcept {
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
            auto result = format_and_output_line(true);
            if (!result) {
                return result;
            }
            line_buffer_.clear();
        }

        // Flush output buffer before form feed
        auto result = flush_output_buffer();
        if (!result) {
            return result;
        }

        // Handle optional page number
        if (!args.empty()) {
            auto page_num = parse_utils::parse_int(args);
            if (page_num) {
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
            auto result = format_and_output_line(true);
            if (!result) {
                return result;
            }
            line_buffer_.clear();
        }

        int lines = 1;
        if (!args.empty()) {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                lines = std::max(0, *parsed);
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        // Output the requested number of blank lines
        for (int i = 0; i < lines; ++i) {
            auto result = format_and_output_line(true);
            if (!result) {
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
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                lines = std::max(0, *parsed);
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        }

        // Flush current line normally before starting centering
        if (!line_buffer_.empty()) {
            auto result = format_and_output_line(true);
            if (!result) {
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
        } else if (starts_with(args, '+') || starts_with(args, '-')) {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                config_.previous_indent = config_.indent;
                config_.indent += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
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
        } else if (starts_with(args, '+') || starts_with(args, '-')) {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                config_.line_length += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
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
        } else if (starts_with(args, '+') || starts_with(args, '-')) {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                config_.temp_indent = config_.indent + *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
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
        } else if (starts_with(args, '+') || starts_with(args, '-')) {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
                config_.page_length += *parsed;
            } else {
                return Result<void>{ErrorCode::InvalidArgument};
            }
        } else {
            auto parsed = parse_utils::parse_int(args);
            if (parsed) {
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
const std::array<std::pair<char, char>, 10> RoffProcessor::escape_mappings_ = {{{'d', '\032'}, {'u', '\035'}, {'r', '\036'}, {'x', '\016'}, {'y', '\017'}, {'l', '\177'}, {'t', '\t'}, {'a', '@'}, {'n', '#'}, {'\\', '\\'}}};

const std::array<std::pair<char, char>, 7> RoffProcessor::prefix_mappings_ = {{{'7', '\036'}, {'8', '\035'}, {'9', '\032'}, {'4', '\b'}, {'3', '\r'}, {'1', '\026'}, {'2', '\027'}}};

} // namespace engine
} // namespace roff

/**
 * @brief Modern C++ main function with comprehensive error handling
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
    auto arg_result = processor.process_arguments(args);
    if (!arg_result) {
        roff::debug::log_error("Error processing arguments: " + std::to_string(static_cast<int>(arg_result.error())));
        return 1;
    }

    // Execute main processing
    auto process_result = processor.process();
    if (!process_result) {
        // Attempt to flush any remaining content before exiting
        [[maybe_unused]] auto flush_result = processor.flush_final_content();

        roff::debug::log_error("Error during processing: " + std::to_string(static_cast<int>(process_result.error())));
        return 1;
    }

    // Ensure all content is properly flushed
    auto flush_result = processor.flush_final_content();
    if (!flush_result) {
        roff::debug::log_error("Error flushing final content: " + std::to_string(static_cast<int>(flush_result.error())));
        return 1;
    }

    return 0;

} catch (const roff::RoffException &e) {
    roff::debug::log_error("ROFF error [" + std::to_string(static_cast<int>(e.code())) + "]: " + e.what() +
                           " at " + e.location().file_name() + ":" + std::to_string(e.location().line()));
    return 2;
} catch (const std::exception &e) {
    roff::debug::log_error("Fatal error: " + std::string{e.what()});
    return 2;
} catch (...) {
    roff::debug::log_error("Unknown fatal error occurred");
    return 3;
}
