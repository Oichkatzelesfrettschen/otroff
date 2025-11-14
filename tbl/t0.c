/* C17 - no scaffold needed */
/* t0.c: storage allocation converted to C++ */
#include <stdio.h>
#include <stdlib.h>
#include "tbl.h"


/* Initialize parser state and compute constant sizes. */
Parser::Parser() {
    texmax = ((int)texstr.size()) - 1;
}

