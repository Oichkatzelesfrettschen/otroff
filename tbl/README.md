# Tbl Preprocessor Sources

`tbl` formats tables prior to processing by `troff` or `nroff`.  This
directory contains the original C sources from the 1977 distribution.
Files `t0.c` through `t9.c` implement the parser and layout engine
with helper modules `ta.c`–`tv.c`.

These files have only been lightly adjusted for modern toolchains and
no new replacement code is included.

Refer to the top-level [README](../README.md) for instructions on
installing dependencies and building the project.
