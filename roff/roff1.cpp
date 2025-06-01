/**
 * @file roff1.cpp
 * @brief Pure C++17 ROFF Text Formatter Implementation
 *
 * A comprehensive ROFF text processor written in modern C++17:
 * - Zero C-style constructs; all classes, structs, and functions utilize standard C++17.
 * - Implements parsing of ROFF control commands (.br, .bp, .in, .ll, .sp, .ce, .fi, .nf, .ad, .na, .so, .nx, .ex).
 * - Uses <charconv>, std::string_view, std::optional, std::unordered_map, std::from_chars, etc.
 * - Fully documented with Doxygen-style comments for each class, struct, and method.
 * - Formatted according to clang-format conventions: 4 spaces per indent, opening braces on same line.
 *
 * @note Be sure to run `clang-format -style=file roff1.cpp` after any edits to maintain formatting.
 *
 * @author Advanced
 * @version 3.1
 * @date 2025-05-30
 */

 #include <iostream>
 #include <fstream>
 #include <string>
 #include <string_view>
 #include <vector>
 #include <algorithm>
 #include <cctype>
 #include <array>
 #include <unordered_map>
 #include <functional>
 #include <exception>
 #include <utility>
 #include <optional>
 #include <filesystem>
 #include <charconv>
 #include <regex>
 #include <numeric>
 #include <string_view> // Added for ""sv literal
 
 /**
  * @brief Exception type for ROFF processor errors.
  *
  * Stores an error code and a descriptive message. Thrown whenever
  * an operation fails (e.g., invalid arguments, file not found, output errors).
  */
 class RoffException : public std::exception
 {
 public:
     enum class ErrorCode
     {
         Success = 0,
         InvalidArgument = 1,
         FileNotFound = 2,
         OutputError = 3,
         InternalError = 4
     };

 private:
     ErrorCode code_;
     std::string message_;
 
 public:
     /**
      * @brief Constructs a RoffException with given code and message.
      * @param code   The ErrorCode indicating the type of failure.
      * @param message  A human-readable description of what went wrong.
      */
     RoffException(ErrorCode code, const std::string &message)
         : code_(code), message_(message) {}
 
     /// Returns the stored message.
     const char* what() const noexcept override { return message_.c_str(); }
 
     /// Returns the stored error code.
     ErrorCode code() const noexcept { return code_; }
 
     /// Location stub (always returns "roff1.cpp" and line 0; can be extended).
     struct source_location
     {
         std::string file_name() const { return "roff1.cpp"; }
         int line() const { return 0; }
     };
 
     /// Returns a dummy source location (future expansions can fill in).
     source_location location() const { return {}; }
 };
 
 /**
  * @struct Config
  * @brief Contains all configurable parameters for RoffProcessor.
  *
  * Configuration includes:
  *  - fill_mode:   if true, lines are filled/wrapped automatically.
  *  - indent, temp_indent: number of spaces to indent.
  *  - line_length: maximum characters per line.
  *  - page_length: maximum lines per page.
  *  - start_page, end_page: page‐range filtering.
  *  - centering_lines_count: how many subsequent lines to center.
  *  - adjust_mode: 0=left, 1=right, 2=center, 3=both (full‐justification).
  */
 struct Config
 {
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
     int adjust_mode = 0;  ///< 0=left, 1=right, 2=center, 3=both
 };
 
 /**
  * @class RoffProcessor
  * @brief Main ROFF processor class implementing parsing and text formatting.
  *
  * Responsibilities:
  *  - Parses ROFF control commands (.br, .bp, .in, .ll, .sp, .ce, .fi, .nf, .ad, .na, .so, .nx, .ex).
  *  - Manages input file streams via std::ifstream (RAII), or stdin if no files specified.
  *  - Handles line-wrapping, indentation, centering, page breaks, fill vs no-fill.
  *  - Uses std::string_view, std::from_chars, and std::optional for argument parsing.
  *  - Throws RoffException on error (invalid arguments, file not found, output failures).
  */
 class RoffProcessor
 {
 public:
     /**
      * @brief Default constructor.
      *        Initializes the processor with default Config values.
      */
     RoffProcessor() = default;
 
     /**
      * @brief Processes command-line arguments for page and file settings.
      * @param args  A vector of std::string representing each argument (e.g., "+2", "-5", "file.roff").
      * @return True on success.
      * @throws RoffException on invalid arguments or file open failures.
      */
     bool process_arguments(const std::vector<std::string> &args)
     {
         for (const auto &arg : args)
         {
             if (arg.empty())
                 continue;
 
             if (arg[0] == '+')
             {
                 auto res = parse_int(arg.substr(1));
                 if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                               "Invalid start page: " + arg);
                 config_.start_page = *res;
             }
             else if (arg[0] == '-')
             {
                 if (arg == "-s" || arg == "-h")
                 {
                     // Stop or high-speed modes; not implemented currently
                 }
                 else
                 {
                     auto res = parse_int(arg.substr(1));
                     if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                                   "Invalid end page: " + arg);
                     config_.end_page = *res;
                 }
             }
             else
             {
                 std::ifstream file{arg}; // Corrected: use arg directly
                 if (!file.is_open())
                 {
                     throw RoffException(RoffException::ErrorCode::FileNotFound,
                                         "Cannot open file: " + arg);
                 }
                 input_files_.emplace_back(std::move(file));
             }
         }
         // If no files were provided, reading will default to stdin.
         return true;
     }
 
     /**
      * @brief Main processing loop.
      *
      * Repeatedly fetches characters and:
      *  - If '.' is found outside of text, invokes process_control_command().
      *  - Otherwise, sends character to process_text_character().
      * After exhausting input, calls flush_final_content() to emit any leftover buffer.
      *
      * @return True on success.
      * @throws RoffException on any I/O or parsing error.
      */
     bool process()
     {
         char ch;
         while (get_next_character(ch))
         {
             if (exit_requested_)
                 break;
 
             if (ch == '.')
             {
                 if (!process_control_command())
                     return false;
             }
             else
             {
                 process_text_character(ch);
             }
         }
 
         flush_final_content();
         return true;
     }
 
 private:
     Config config_;                                 ///< Holds all user-configurable state
     std::string line_buffer_;                       ///< Buffer for current line of text
     std::vector<std::ifstream> input_files_;        ///< Vector of input file streams (RAII)
     std::size_t current_file_index_ = 0;            ///< Index of file currently being read
     bool exit_requested_ = false;                   ///< Set to true when `.ex` command encountered
     int current_page_ = 1;                          ///< Current page number
     int current_line_in_page_ = 0;                  ///< Number of lines output on current page
 
     /**
      * @brief Fetch next character from input stream(s) or stdin.
      *
      * Reads from the current file in input_files_. When EOF is reached on that file,
      * advances to the next file. If no files were provided or all are exhausted,
      * reads from stdin. Returns false when exit_requested_ is true or no more characters.
      *
      * @param[out] ch  The character that was read.
      * @return true if a character was read; false otherwise.
      */
     bool get_next_character(char &ch)
     {
         if (exit_requested_)
             return false;
 
         // Attempt to read from each file in turn
         while (current_file_index_ < input_files_.size())
         {
             auto &file = input_files_[current_file_index_];
             if (file.get(ch))
                 return true;
             ++current_file_index_;
         }
 
         // If no file remains or no files were provided, read from stdin
         if (input_files_.empty() || current_file_index_ >= input_files_.size())
         {
             if (std::cin.get(ch))
                 return true;
         }
         return false;
     }
 
     /**
      * @brief Processes a control command, which always begins with '.'.
      *
      * Reads two characters to form a 2-letter command (e.g., "br") and then
      * accumulates everything up to the newline as 'args'. Trims whitespace and
      * dispatches to the matching command_*() handler via execute_command().
      *
      * @return true on success; false on EOF or failure.
      * @throws RoffException on invalid arguments.
      */
     bool process_control_command()
     {
         char cmd1, cmd2;
         if (!get_next_character(cmd1) || !get_next_character(cmd2))
             return false;
 
         std::string command;
         command += cmd1;
         command += cmd2;
 
         std::string args;
         char ch;
         while (get_next_character(ch) && ch != '\n')
             args.push_back(ch);
 
         std::string_view trimmed_args = trim(args);
         return execute_command(std::string_view(command), trimmed_args);
     }
 
     /**
      * @brief Dispatches a 2-letter command to the appropriate handler.
      *
      * Uses a static std::unordered_map<std::string_view, function<…>> for fast lookup.
      * If the command is not recognized, it is silently ignored (per ROFF conventions).
      *
      * @param command Two-letter command string.
      * @param args    The trimmed string_view of arguments following the command.
      * @return true on success.
      * @throws RoffException if the handler throws (e.g., invalid argument parsing).
      */
     bool execute_command(std::string_view command, std::string_view args)
     {
         static const std::unordered_map<std::string_view,
             std::function<bool(RoffProcessor *, std::string_view)>> command_map =
         {
             { "br", [](RoffProcessor *self, std::string_view a) { return self->command_break_line(); } },
             { "bp", [](RoffProcessor *self, std::string_view a) { return self->command_break_page(a); } },
             { "sp", [](RoffProcessor *self, std::string_view a) { return self->command_space_lines(a); } },
             { "ce", [](RoffProcessor *self, std::string_view a) { return self->command_center_lines(a); } },
             { "fi", [](RoffProcessor *self, std::string_view)    { self->config_.fill_mode = true; return true; } },
             { "nf", [](RoffProcessor *self, std::string_view)    { self->config_.fill_mode = false; return true; } },
             { "in", [](RoffProcessor *self, std::string_view a) { return self->command_set_indent(a); } },
             { "ll", [](RoffProcessor *self, std::string_view a) { return self->command_set_line_length(a); } },
             { "ti", [](RoffProcessor *self, std::string_view a) { return self->command_temporary_indent(a); } },
             { "pl", [](RoffProcessor *self, std::string_view a) { return self->command_set_page_length(a); } },
             { "ad", [](RoffProcessor *self, std::string_view a) { return self->command_adjust_text(a); } },
             { "na", [](RoffProcessor *self, std::string_view)    { self->config_.adjust_mode = 0; return true; } },
             { "so", [](RoffProcessor *self, std::string_view a) { return self->command_source_file(a); } },
             { "nx", [](RoffProcessor *self, std::string_view a) { return self->command_next_file(a); } },
             { "ex", [](RoffProcessor *self, std::string_view)    { self->exit_requested_ = true; return true; } }
         };
 
         auto it = command_map.find(command);
         if (it != command_map.end())
             return it->second(this, args);
 
         // Unknown commands are ignored
         return true;
     }
 
     /**
      * @brief Processes a regular text character (non-control).
      *
      * 1. Applies escape sequences via process_escape_sequences().
      * 2. Skips output if current_page_ is outside [start_page, end_page].
      * 3. Checks for page_length overflow; if so, performs page break.
      * 4. If character is '\n', calls process_newline(); otherwise, appends to line_buffer_.
      * 5. If in fill_mode and line_buffer_ length ≥ line_length, calls handle_line_wrap().
      *
      * @param ch  The character to process.
      * @throws RoffException on formatting or output errors.
      */
     void process_text_character(char ch)
     {
         ch = process_escape_sequences(ch);
 
         if (is_outside_page_range())
             return;
 
         if (config_.page_length > 0 && current_line_in_page_ >= config_.page_length)
         {
             command_break_page(std::string_view{""}); // Replaced ""sv
         }
 
         if (ch == '\n')
         {
             process_newline();
         }
         else
         {
             line_buffer_.push_back(ch);
             if (config_.fill_mode && should_wrap_line())
                 handle_line_wrap();
         }
     }
 
     /**
      * @brief Converts simple escape sequences ('\t', '\n', '\\') to their actual char.
      *
      * If the current character is '\\', peeks ahead one char and transforms:
      *  - "\\t" → '\t'
      *  - "\\n" → '\n'
      *  - "\\\\" → '\\'
      * Otherwise, returns the original character.
      *
      * @param ch  Input character.
      * @return Transformed character or original if not an escape pattern.
      */
     char process_escape_sequences(char ch)
     {
         if (ch == '\\')
         {
             char next;
             if (get_next_character(next))
             {
                 switch (next)
                 {
                     case 't':  return '\t';
                     case 'n':  return '\n';
                     case '\\': return '\\';
                     default:   return next;
                 }
             }
         }
         return ch;
     }
 
     /**
      * @brief Checks whether current_page_ is outside [start_page, end_page].
      *
      * @return true if current_page_ < start_page or (end_page > 0 && current_page_ > end_page).
      */
     bool is_outside_page_range() const noexcept
     {
         return (current_page_ < config_.start_page) ||
                (config_.end_page > 0 && current_page_ > config_.end_page);
     }
 
     /**
      * @brief Handles a newline character: outputs the line_buffer_ as a formatted line.
      *
      * 1. Calls output_line() to apply indentation/centering/justification.
      * 2. Clears line_buffer_ for next line.
      *
      * @throws RoffException on output failure.
      */
     void process_newline()
     {
         output_line();
         line_buffer_.clear();
     }
 
     /**
      * @brief Determines if the current line_buffer_ has reached or exceeded line_length.
      *
      * @return true if line_buffer_.length() ≥ config_.line_length.
      */
     bool should_wrap_line() const noexcept
     {
         return (config_.line_length > 0) &&
                (static_cast<int>(line_buffer_.length()) >= config_.line_length);
     }
 
     /**
      * @brief Handles word-wrap in fill_mode.
      *
      * Splits line_buffer_ at the last space ≤ line_length. If no space, breaks mid-word.
      * 1. Part before break is output via output_line().
      * 2. Remainder (after break) becomes new line_buffer_.
      *
      * @throws RoffException on output failure.
      */
     void handle_line_wrap()
     {
         std::size_t break_pos = line_buffer_.rfind(' ', static_cast<std::size_t>(config_.line_length));
         if (break_pos == std::string::npos || break_pos == 0)
         {
             break_pos = static_cast<std::size_t>(config_.line_length);
         }
         std::string part_to_output  = line_buffer_.substr(0, break_pos);
         std::string remainder       = (break_pos + 1 < line_buffer_.size())
                                      ? line_buffer_.substr(break_pos + 1)
                                      : "";
 
         // Temporarily set line_buffer_ to part_to_output, output it, then restore remainder
         line_buffer_ = part_to_output;
         output_line();
         line_buffer_ = remainder;
     }
 
     /**
      * @brief Outputs the current line_buffer_ respecting indent and centering.
      *
      * 1. If line_buffer_ is empty, simply outputs a newline and increments current_line_in_page_.
      * 2. Otherwise, calls format_line() to apply indentation and centering.
      * 3. Writes the result to stdout with `std::cout`.
      * 4. Increments current_line_in_page_, clears temp-indent flag if set, decrements centering counter if >0.
      *
      * @throws RoffException if writing to stdout fails.
      */
     void output_line()
     {
         if (line_buffer_.empty())
         {
             std::cout << '\n';
             ++current_line_in_page_;
             return;
         }
 
         std::string formatted = format_line(line_buffer_);
         std::cout << formatted << '\n';
         if (std::cout.fail())
             throw RoffException(RoffException::ErrorCode::OutputError,
                                 "Failed writing output line.");
 
         ++current_line_in_page_;
 
         if (config_.apply_temp_indent_once)
             config_.apply_temp_indent_once = false;
 
         if (config_.centering_lines_count > 0)
             --config_.centering_lines_count;
     }
 
     /**
      * @brief Applies indentation and centering to a single line of text.
      *
      * 1. If a temporary indent is pending, uses temp_indent; otherwise uses indent.
      * 2. If centering_lines_count > 0, computes padding = (line_length − line.size())/2
      *    and prepends that many spaces.
      * 3. Returns the final string with leading spaces applied.
      *
      * @param line  The raw text line to format.
      * @return A new string containing indentation and centering.
      */
     std::string format_line(const std::string &line)
     {
         std::string result = line;
         int indent_value = config_.apply_temp_indent_once ? config_.temp_indent : config_.indent;
 
         if (indent_value > 0)
             result.insert(0, static_cast<std::size_t>(indent_value), ' ');
 
         if (config_.centering_lines_count > 0)
         {
             int padding = std::max(0, (config_.line_length - static_cast<int>(result.length())) / 2);
             result.insert(0, static_cast<std::size_t>(padding), ' ');
         }
         return result;
     }
 
     /**
      * @brief Flushes any leftover content in line_buffer_ after input is exhausted.
      *
      * If line_buffer_ is nonempty, calls output_line() once more.
      *
      * @throws RoffException if writing fails.
      */
     void flush_final_content()
     {
         if (!line_buffer_.empty())
             output_line();
     }
 
     /**
      * @brief Handle the ".br" (break line) control command.
      *
      * Immediately outputs the current line_buffer_ (via output_line()) and clears it.
      *
      * @return true on success.
      */
     bool command_break_line()
     {
         output_line();
         line_buffer_.clear();
         return true;
     }
 
     /**
      * @brief Handle the ".bp" (break page) control command.
      *
      * 1. If line_buffer_ is not empty, outputs it and clears the buffer.
      * 2. If args is nonempty, parses it as an integer to set current_page_; otherwise increments current_page_.
      * 3. Resets current_line_in_page_ to 0.
      * 4. Emits a form-feed character '\f'.
      *
      * @param args  Numeric string specifying new page number, or empty.
      * @return true on success.
      * @throws RoffException if args is invalid or output fails.
      */
     bool command_break_page(std::string_view args)
     {
         if (!line_buffer_.empty())
         {
             output_line();
             line_buffer_.clear();
         }
 
         if (!args.empty())
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid page number: " + std::string(args));
             current_page_ = *res;
         }
         else
         {
             ++current_page_;
         }
 
         current_line_in_page_ = 0;
         std::cout << '\f';
         if (std::cout.fail())
             throw RoffException(RoffException::ErrorCode::OutputError,
                                 "Failed writing form feed.");
 
         return true;
     }
 
     /**
      * @brief Handle the ".sp" (space lines) control command.
      *
      * 1. If line_buffer_ is not empty, outputs it and clears it.
      * 2. Parses args as integer (or uses 1 if empty).
      * 3. Outputs that many blank lines (calls output_line() repeatedly).
      *
      * @param args  Number of blank lines, or empty → 1.
      * @return true on success.
      * @throws RoffException if args invalid or output fails.
      */
     bool command_space_lines(std::string_view args)
     {
         if (!line_buffer_.empty())
         {
             output_line();
             line_buffer_.clear();
         }
 
         int lines = 1;
         if (!args.empty())
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid space count: " + std::string(args));
             lines = *res;
         }
 
         for (int i = 0; i < lines; ++i)
             output_line();
 
         return true;
     }
 
     /**
      * @brief Handle the ".ce" (center lines) control command.
      *
      * 1. Parses args as integer → number of lines to center (or uses 1 if empty).
      * 2. If line_buffer_ is not empty, outputs it and clears it.
      * 3. Sets config_.centering_lines_count to that number. The next N calls to output_line()
      *    will prepend appropriate spaces to center the text.
      *
      * @param args  Number of lines to center, or empty → 1.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_center_lines(std::string_view args)
     {
         int lines = 1;
         if (!args.empty())
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid center count: " + std::string(args));
             lines = *res;
         }
 
         if (!line_buffer_.empty())
         {
             output_line();
             line_buffer_.clear();
         }
 
         config_.centering_lines_count = lines;
         return true;
     }
 
     /**
      * @brief Handle the ".in" (set indent) control command.
      *
      * 1. If args is empty, restores previous_indent.
      * 2. If args starts with '+' or '-', adjusts indent by that integer.
      * 3. Otherwise, sets indent = parsed int.
      * 4. Ensures indent ≥ 0.
      *
      * @param args  Indent specification string.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_set_indent(std::string_view args)
     {
         if (args.empty())
         {
             config_.indent = config_.previous_indent;
         }
         else if (args[0] == '+' || args[0] == '-')
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid indent: " + std::string(args));
             config_.previous_indent = config_.indent;
             config_.indent += *res;
         }
         else
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid indent: " + std::string(args));
             config_.previous_indent = config_.indent;
             config_.indent = *res;
         }
 
         config_.indent = std::max(0, config_.indent);
         return true;
     }
 
     /**
      * @brief Handle the ".ll" (set line length) control command.
      *
      * 1. If args is empty, resets to default (65).
      * 2. If args starts with '+' or '-', adjusts line_length.
      * 3. Otherwise, sets line_length = parsed int.
      * 4. Ensures line_length ≥ 10.
      *
      * @param args  Line length specification.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_set_line_length(std::string_view args)
     {
         if (args.empty())
         {
             config_.line_length = 65;
         }
         else if (args[0] == '+' || args[0] == '-')
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid line length: " + std::string(args));
             config_.line_length += *res;
         }
         else
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid line length: " + std::string(args));
             config_.line_length = *res;
         }
 
         config_.line_length = std::max(10, config_.line_length);
         return true;
     }
 
     /**
      * @brief Handle the ".ti" (temporary indent) control command.
      *
      * 1. If args is empty, sets temp_indent = 0.
      * 2. If args starts with '+' or '-', sets temp_indent = indent + parsed delta.
      * 3. Otherwise, sets temp_indent = parsed int.
      * 4. Ensures temp_indent ≥ 0, and sets apply_temp_indent_once = true.
      *
      * @param args  Temporary indent specification.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_temporary_indent(std::string_view args)
     {
         if (args.empty())
         {
             config_.temp_indent = 0;
         }
         else if (args[0] == '+' || args[0] == '-')
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid temporary indent: " + std::string(args));
             config_.temp_indent = config_.indent + *res;
         }
         else
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid temporary indent: " + std::string(args));
             config_.temp_indent = *res;
         }
 
         config_.temp_indent = std::max(0, config_.temp_indent);
         config_.apply_temp_indent_once = true;
         return true;
     }
 
     /**
      * @brief Handle the ".pl" (set page length) control command.
      *
      * 1. If args is empty, resets to default (66).
      * 2. If args starts with '+' or '-', adjusts page_length.
      * 3. Otherwise, sets page_length = parsed int.
      * 4. Ensures page_length ≥ 0.
      *
      * @param args  Page length specification.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_set_page_length(std::string_view args)
     {
         if (args.empty())
         {
             config_.page_length = 66;
         }
         else if (args[0] == '+' || args[0] == '-')
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid page length: " + std::string(args));
             config_.page_length += *res;
         }
         else
         {
             auto res = parse_int(args);
             if (!res) throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                           "Invalid page length: " + std::string(args));
             config_.page_length = *res;
         }
 
         config_.page_length = std::max(0, config_.page_length);
         return true;
     }
 
     /**
      * @brief Handle the ".ad" (adjust text) control command.
      *
      * 1. Parses args:
      *    - empty or "b"/"B" → adjust_mode = 3 (both justification).
      *    - "l"/"L" → adjust_mode = 0 (left).
      *    - "r"/"R" → adjust_mode = 1 (right).
      *    - "c"/"C" → adjust_mode = 2 (center).
      * 2. On invalid args, throws RoffException.
      *
      * @param args  Adjustment mode string.
      * @return true on success.
      * @throws RoffException if args invalid.
      */
     bool command_adjust_text(std::string_view args)
     {
         if (args.empty() || args == "b" || args == "B")
             config_.adjust_mode = 3;
         else if (args == "l" || args == "L")
             config_.adjust_mode = 0;
         else if (args == "r" || args == "R")
             config_.adjust_mode = 1;
         else if (args == "c" || args == "C")
             config_.adjust_mode = 2;
         else
             throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                  "Invalid adjust mode: " + std::string(args));
         return true;
     }
 
     /**
      * @brief Handle the ".so" (source file) control command.
      *
      * 1. If args is empty, throws an exception.
      * 2. Opens the new file and inserts its stream immediately after the current file.
      * 3. If file open fails, throws RoffException.
      *
      * @param args  Filename to open and source next.
      * @return true on success.
      * @throws RoffException on invalid filename or open failure.
      */
     bool command_source_file(std::string_view args)
     {
         if (args.empty())
             throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                  "No file specified for .so command");
 
         std::ifstream file{std::string(args)};
         if (!file.is_open())
             throw RoffException(RoffException::ErrorCode::FileNotFound,
                                  "Cannot open file: " + std::string(args));
 
         auto insert_pos = input_files_.begin() + static_cast<std::ptrdiff_t>(current_file_index_) + 1;
         input_files_.insert(insert_pos, std::move(file));
         return true;
     }
 
     /**
      * @brief Handle the ".nx" (next file) control command.
      *
      * 1. If args is empty, throws an exception.
      * 2. Closes any currently opened file (if still open).
      * 3. Clears the entire input_files_ list.
      * 4. Opens the new file and appends to input_files_.
      * 5. current_file_index_ is reset to 0.
      *
      * @param args  Filename to switch to immediately.
      * @return true on success.
      * @throws RoffException on invalid filename or open failure.
      */
     bool command_next_file(std::string_view args)
     {
         if (args.empty())
             throw RoffException(RoffException::ErrorCode::InvalidArgument,
                                  "No file specified for .nx command");
 
         if (current_file_index_ < input_files_.size())
         {
             auto &current_file = input_files_[current_file_index_];
             if (current_file.is_open())
                 current_file.close();
         }
 
         input_files_.clear();
         current_file_index_ = 0;
 
         std::ifstream file{std::string(args)};
         if (!file.is_open())
             throw RoffException(RoffException::ErrorCode::FileNotFound,
                                  "Cannot open file: " + std::string(args));
 
         input_files_.push_back(std::move(file));
         return true;
     }
 
     /**
      * @brief Trims whitespace from both ends of a string and returns a std::string_view.
      *
      * @param str  Input string to trim.
      * @return std::string_view pointing to trimmed substring.
      */
     static std::string_view trim(const std::string &str)
     {
         const auto start = str.find_first_not_of(" \t\r\n");
         if (start == std::string::npos)
             return {};
         const auto end = str.find_last_not_of(" \t\r\n");
         return std::string_view(str).substr(start, end - start + 1);
     }
 
     /**
      * @brief Parses a signed integer from a string_view using std::from_chars.
      *
      * If parsing succeeds and the entire string_view is consumed, returns optional<int>.
      * Otherwise returns std::nullopt.
      *
      * @param sv  The string_view containing digits, possibly with a '+' or '-'.
      * @return std::optional<int>  Parsed integer or std::nullopt on failure.
      */
     static std::optional<int> parse_int(std::string_view sv)
     {
         // Convert to a null-terminated string for from_chars
         std::string temp(sv);
         int value = 0;
         const char *first = temp.data();
         const char *last  = temp.data() + temp.size();
         auto result        = std::from_chars(first, last, value);
         if (result.ec == std::errc() && result.ptr == last)
             return value;
         return std::nullopt;
     }
 };  // end class RoffProcessor
 
 /**
  * @brief Main entry point.
  *
  * 1. Converts C‐style argv[] to std::vector<std::string>.
  * 2. Creates a RoffProcessor instance and calls process_arguments().
  * 3. Calls process() to do all input parsing and output generation.
  * 4. Catches and prints any RoffException that bubbles up.
  *
  * @param argc  Number of command‐line arguments.
  * @param argv  C‐string array of arguments.
  * @return 0 on success; nonzero on failure.
  */
 int main(int argc, char *argv[]) try
 {
     // Build a vector of std::string from argv[1..]
     std::vector<std::string> args;
     args.reserve((argc > 1) ? static_cast<std::size_t>(argc - 1) : 0);
     for (int i = 1; i < argc; ++i)
         args.emplace_back(argv[i]);
 
     // Instantiate and configure the processor
     RoffProcessor processor;
     processor.process_arguments(args);
 
     // Begin main parsing & formatting loop
     processor.process();
     return 0;
 }
 catch (const RoffException &ex)
 {
     std::cerr << "[ROFF ERROR] " << ex.what() << "\n";
     return static_cast<int>(ex.code());
 }