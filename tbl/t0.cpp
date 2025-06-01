#include "../cxx17_scaffold.hpp"
/* t0.c: storage allocation converted to C++ */
#include <cstdio>
#include <cstdlib>
#include "tbl.hpp"

namespace tbl {

/* Initialize parser state and compute constant sizes. */
Parser::Parser() {
    texmax = static_cast<int>(texstr.size()) - 1;
}

} // namespace tbl
