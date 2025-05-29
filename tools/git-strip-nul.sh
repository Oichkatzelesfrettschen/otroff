#!/usr/bin/env sh
# git-strip-nul.sh -- remove trailing NUL bytes from input
#
# This script is intended for use as a Git filter. It reads data from
# standard input, strips any NUL characters that appear at the end of
# the stream, and writes the cleaned result to standard output. A final
# newline is always appended so editors see a proper end of file.

set -e

# Use Perl for reliable byte processing. The regular expression
# replaces one or more NUL bytes at the end of the data with a single
# newline character.
exec perl -0pe 's/\x00+\z/\n/'
