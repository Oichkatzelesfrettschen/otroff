# Nroff Formatter Sources

This directory contains the C implementation of `nroff`, the terminal-oriented
counterpart to `troff`.  The `.c`, `.h` and `.s` files are the original PWB/UNIX
sources dating back to 1977.  Shell scripts such as `bothrc`, `nrc` and
`sufrc` are the historic build drivers.

Minor tweaks were applied so the code will compile on modern systems, but
no contemporary replacements are provided here.  The `term/` subdirectory
holds the device tables used by `nroff` for various terminals.

See the top-level [README](../README.md) for instructions on preparing the
build environment and invoking `make`.
