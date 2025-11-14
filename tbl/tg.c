/* C17 - no scaffold needed */
/* tg.c: process included text blocks */
#include "tbl.h"

/* Retrieve a text block diversion. */
int gettext(char *sp, int ilin, int icol, char *fn, char *sz) {
    /* get a section of text */
    char line[256];
    int oname;
    if (texname == 0)
        error("Too many text block diversions");
    fprintf(tabout, ".br\n");
    fprintf(tabout, ".di %c+\n", texname);
    if (textflg == 0) {
        fprintf(tabout, ".nr %d \\n(.lu\n", TMP); /* remember old line length */
        textflg = 1;
    }
    rstofill();
    if (fn && *fn)
        fprintf(tabout, ".nr %d \\n(.f\n.ft %s\n", S1, fn);
    if (sz && *sz)
        fprintf(tabout, ".ps %s\n.vs \\n(.s+2\n", sz);
    if (cll[icol][0])
        fprintf(tabout, ".ll %sn\n", cll[icol]);
    else
        fprintf(tabout, ".ll \\n(%du*%du/%du\n", TMP, ctspan(ilin, icol), ncol + 1);
    if (ctype(ilin, icol) == 'a')
        fprintf(tabout, ".ll -2n\n");
    while (gets1(line)) {
        if (line[0] == 'T' && line[1] == '}' && line[2] == tab)
            break;
        if (match("T}", line))
            break;
        fprintf(tabout, "%s\n", line);
    }
    if (fn && *fn)
        fprintf(tabout, ".ft \\n(%d\n", S1);
    if (sz && *sz)
        fprintf(tabout, ".br\n.ps\n.vs\n");
    fprintf(tabout, ".br\n");
    fprintf(tabout, ".di\n");
    fprintf(tabout, ".nr %c| \\n(dn\n", texname);
    fprintf(tabout, ".nr %c- \\n(dl\n", texname);
    /* copy remainder of line */
    if (line[2])
        tcopy(sp, line + 3);
    else
        *sp = 0;
    oname = texname;
    texname = texstr[++texct];
    return (oname);
}
/* Restore normal line length after text block. */
void untext(void) {
    fprintf(tabout, ".nf\n");
    fprintf(tabout, ".ll \\n(%du\n", TMP);
}
