# Neqn Preprocessor Sources

`neqn` is the equation formatter adapted for `nroff`.  This directory holds
the original PWB/UNIX sources from 1977.  Files `ne0.c` through `ne6.c` form
the core engine, while `ne.g` and `nelex.c` provide the grammar and lexical
scanner.  The `neqnrc` script is the historical build wrapper.

The code is largely unmodified aside from minor fixes for modern C
compilers.  No contemporary helper routines are supplied here.

For build instructions see the project's [README](../README.md).
