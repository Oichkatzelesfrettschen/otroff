#include "../cxx17_scaffold.hpp"
#include "tbl.hpp"

#include <cstdlib>
#include <cstdio>

namespace tbl {
// Execute all stages needed for a single table.
void tableput() {
    saveline();
    savefill();
    ifdivert();
    cleanfc();
    getcomm();
    getspec();
    gettbl();
    getstop();
    checkuse();
    choochar();
    maktab();
    runout();
    release();
    rstofill();
    endoff();
    restline();
}
} // namespace tbl
