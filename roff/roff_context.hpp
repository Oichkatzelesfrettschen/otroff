#pragma once

#include <string> // For potential string members later
#include <vector> // For potential vector members later
#include <array>  // For fixed-size arrays like 'line' or 'word'

// Forward declare if complex types from legacy code are needed by pointer/reference
// namespace otroff { namespace roff_legacy {
//   // Forward declarations if needed
// }}

namespace otroff {
namespace roff {

// Define constants that were part of roff.hpp or roff8.cpp if they are truly constant
// and relevant to the context class structure (e.g. buffer sizes).
// These should ideally be scoped within the class or this namespace.
constexpr int LEGACY_LINE_SIZE = 256; // Example, use actual from roff.hpp
constexpr int LEGACY_WORD_SIZE = 64;  // Example, use actual from roff.hpp
constexpr int LEGACY_FILENAME_SIZE = 20; // Example
constexpr int LEGACY_TAB_TABLE_SIZE = 10; // Example

class RoffContext {
public:
    RoffContext(); // Constructor
    ~RoffContext(); // Destructor

    // === Page Layout Parameters ===
    int pn;          // Current page number
    int ls;          // Line spacing value
    int ll;          // Line length setting
    int pl;          // Page length in lines
    int po;          // Page offset (left margin)
    int ma1, ma2, ma3, ma4;  // Margins
    int bl;          // Bottom line number (computed from pl and margins)
    int hx;          // Header/footer processing flag (1=yes, 0=no)
    // char* ehead;  // Example: will become std::string or similar
    // char* ohead;
    // char* efoot;
    // char* ofoot;

    // === Text Processing State ===
    int fi;          // Fill mode flag (1=fill, 0=no-fill)
    int ad;          // Adjust mode flag (1=justify, 0=left-align)
    int in;          // Permanent indent value
    int un;          // Temporary indent value
    int cc;          // Command character (e.g., '.')
    int nlflg;       // Newline flag from input processing
    int ch;          // Current character from input (buffered)

    // === Buffers (will likely become std::vector<char> or std::string later) ===
    // For now, keep as arrays to mirror legacy, but plan for change.
    std::array<char, LEGACY_LINE_SIZE> line_buffer;
    std::array<char, LEGACY_WORD_SIZE> word_buffer;
    char* linep;     // Pointer into line_buffer
    char* wordp;     // Pointer into word_buffer

    // Example of a more complex global - input buffer file descriptor
    int ibf;         // Input buffer file descriptor (for macros/diversions)

    // Input processing state (related to getch)
    // int ch; // Already present
    // int nlflg; // Already present
    int ch0;                // Next character for getch
    int cwidth;             // Width of last character from getch
    int level;              // Nesting level for getch
    int h_pos;              // Current horizontal position on input line (v.hp)

    // Word processing state (already partially in context, ensure these are covered)
    int wch;                // Word character count
    int wne;                // Word width in units

    // Escape character and related
    int eschar;             // Escape character itself (e.g., '\\')
    int copyf;              // Copy mode flag (used by getch)

    // Font/Size (getch calculates width based on chbits, which depends on font/size)
    int current_font_ps;    // Represents point size part of chbits
    int current_font_ft;    // Represents font family part of chbits
    int chbits;             // Character formatting bits (font, size)

    // Input Buffer State (related to getch0)
    std::vector<char> input_buffer_storage; // Main input buffer
    char* ibufp;           // Current pointer within input_buffer_storage
    char* eibuf;           // End pointer for input_buffer_storage

    int current_ifile;     // Replaces global 'ifile'

    // String/macro input pointers (simplified for now)
    const char* string_input_ptr; // For reading from a string (e.g. macro expansion)
    const char* string_input_end;

    // TODO: Add stack for include files/macros (ip, ap, cp stack) later

    // Line state & formatting parameters (some might be duplicates from above, ensure single definition)
    // int ll;         // Line length - already in RoffContext
    // int un;         // Temporary indent - already in RoffContext
    // char* linep;    // Pointer into line_buffer - already in RoffContext
    // std::array<char, LEGACY_LINE_SIZE> line_buffer; // Already in RoffContext
    // int nwd;     // Number of words - already in RoffContext
    // int in;      // Indent - already in RoffContext
    // int bl;      // Bottom line - already in RoffContext
    int nl;         // Current line number on page (distinct from RoffContext.pn page number)
    int nc;         // Character count in current line buffer
    int ne;         // Width units in current line buffer (typographical units)
    int nel;        // Number of characters or units left on line

    // Justification factors
    int fac;
    int fmq;

    // For rbreak and its callees
    int totout;      // Total output lines counter
    // int ls;       // Line spacing - already in RoffContext
    int ls1;        // Saved line spacing (for .ls command)
    int skip;       // Lines to skip at next page break (used by rbreak via eject)

    // Header/Footer pointers (Store as std::string for proper memory management eventually)
    // For now, as char* to mirror legacy state closely during this refactoring phase.
    // Initialized to nullptr. Actual strings would be allocated/assigned by command handlers.
    char* ehead_ptr; // Even page header string
    char* ohead_ptr; // Odd page header string
    char* efoot_ptr; // Even page footer string
    char* ofoot_ptr; // Odd page footer string

    // === Output state (related to putchar_roff) ===
    int nsp;      // Pending space count for output
    int ocol;     // Current output column

    // === Word/Line state ===
    int nwd;      // Number of words in current line

    // === Mode flags ===
    int slow;     // Slow output mode flag (for putchar_roff)
    int pfrom;    // Starting page number for printing
    int pto;      // Ending page number for printing

    // Buffer for output - consistent with LEGACY_LINE_SIZE for now
    std::array<char, LEGACY_LINE_SIZE> output_buffer; // Using LEGACY_LINE_SIZE as defined before
    char* obufp;   // Pointer into output_buffer


    // Add more members here incrementally from roff8.cpp's list.
    // This initial subtask will only include a few key ones.

private:
    // Helper methods for initialization or internal state if needed
    void initialize_buffers();
    void compute_bottom_line();

    // Potentially other private members if we decompose state further internally

public:
    // Method to load data into the main input buffer
    void load_main_input_buffer(const std::vector<char>& data);
};

// Declaration for the contextualized function
void putchar_roff_contextual(RoffContext& context, int c);
int getch_contextual(RoffContext& context);
int getch0_contextual(RoffContext& context);
void setnel_contextual(RoffContext& context);
void rbreak_contextual(RoffContext& context);

// New contextual function declarations
void newline_contextual(RoffContext& context);
void nline_contextual(RoffContext& context);
void space_contextual(RoffContext& context, int count);

} // namespace roff
} // namespace otroff
