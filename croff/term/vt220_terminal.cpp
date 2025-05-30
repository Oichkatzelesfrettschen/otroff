#include "vt220_terminal.hpp"
#include <stdexcept>
#include <algorithm>

namespace otroff::terminal {

// Global instance for C compatibility
static const VT220Terminal g_vt220_instance{};

} // namespace otroff::terminal

// C-compatible interface for legacy code
extern "C" {

struct termtab {
    int bset;
    int breset;
    int Hor;
    int Vert;
    int Newline;
    int Char;
    int Em;
    int Halfline;
    int Adj;
    const char *twinit;
    const char *twrest;
    const char *twnl;
    const char *hlr;
    const char *hlf;
    const char *flr;
    const char *bdon;
    const char *bdoff;
    const char *ploton;
    const char *plotoff;
    const char *up;
    const char *down;
    const char *right;
    const char *left;
    const char *codetab[256 - 32];
    int zzz;
};

// Legacy C interface
const struct termtab* get_vt220_table() {
    static struct termtab legacy_table = {};
    static bool initialized = false;

    if (!initialized) {
        const auto& terminal = otroff::terminal::g_vt220_instance;
        const auto& caps = terminal.capabilities();
        const auto& seqs = terminal.control_sequences();

        legacy_table.bset = caps.bset;
        legacy_table.breset = caps.breset;
        legacy_table.Hor = caps.horizontal_resolution;
        legacy_table.Vert = caps.vertical_resolution;
        legacy_table.Newline = caps.newline_spacing;
        legacy_table.Char = caps.char_width;
        legacy_table.Em = caps.em_width;
        legacy_table.Halfline = caps.halfline_spacing;
        legacy_table.Adj = caps.adjustment_spacing;

        legacy_table.twinit = seqs.init.data();
        legacy_table.twrest = seqs.reset.data();
        legacy_table.twnl = seqs.newline.data();
        legacy_table.hlr = seqs.halfline_reverse.data();
        legacy_table.hlf = seqs.halfline_forward.data();
        legacy_table.flr = seqs.fullline_reverse.data();
        legacy_table.bdon = seqs.bold_on.data();
        legacy_table.bdoff = seqs.bold_off.data();
        legacy_table.ploton = seqs.plot_on.data();
        legacy_table.plotoff = seqs.plot_off.data();
        legacy_table.up = seqs.cursor_up.data();
        legacy_table.down = seqs.cursor_down.data();
        legacy_table.right = seqs.cursor_right.data();
        legacy_table.left = seqs.cursor_left.data();

        // Convert code table
        const auto code_table = terminal.code_table();
        for (size_t i = 0; i < std::min(code_table.size(), size_t{224}); ++i) {
            legacy_table.codetab[i] = code_table[i].representation.data();
        }

        legacy_table.zzz = 0;
        initialized = true;
    }

    return &legacy_table;
}

} // extern "C"
