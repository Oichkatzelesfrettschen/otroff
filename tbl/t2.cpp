#include "cxx23_scaffold.hpp"
#include "tbl.h"

#include <cstdlib>
#include <cstdio>

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
