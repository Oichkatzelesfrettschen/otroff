# Tbl Preprocessor Sources

`tbl` formats tables prior to processing by `troff` or `nroff`.  This
directory contains the original C sources from the 1977 distribution.
Files `t0.c` through `t9.c` implement the parser and layout engine
with helper modules `ta.c`–`tv.c`.

These files have only been lightly adjusted for modern toolchains and
no new replacement code is included.
This repository is a work in progress to reproduce the original MINIX simplicity on modern 32-bit and 64-bit ARM and x86/x86_64 hardware using C++23.


Refer to the top-level [README](../README.md) for instructions on
installing dependencies and building the project.
