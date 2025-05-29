# PDP-11 Assembly Components

This directory contains the original PDP-11 assembly sources for the
`troff` text formatter.  Each file implements a portion of the historic
formatter.

| File | Purpose |
|------|---------|
| `roff1.s` | Program start‑up, character input routines and output buffering.
  Includes utilities such as `mesg` and `flush`. |
| `roff2.s` | Implementation of basic requests such as `.ad`, `.bp` and
  various control functions. |
| `roff3.s` | Additional formatter logic used by `roff2.s`. Handles command
  parsing and escape processing. |
| `roff4.s` | Low‑level text processing: filling, justification and handling
  of regular input lines. |
| `roff5.s` | Hyphenation driver and helper routines used to determine
  potential break points within words. |
| `roff7.s` | Data tables for the hyphenation algorithm (digram tables). |
| `roff8.s` | Global variables and initialised data required by the
  formatter. |

These sources remain for reference and historical interest.  Modern
systems can compile the C replacements that reside in this directory
instead of these PDP‑11 assembly files.

## C translations

Minimal C counterparts exist for the historical assembly sources.  Files
`roff1.c` through `roff8.c` provide skeletal implementations suitable for
building on modern systems.  They only demonstrate a small portion of the
original behaviour but allow the code to compile without a PDP‑11 toolchain.
