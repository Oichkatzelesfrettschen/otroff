#!/usr/bin/env python3
"""Simple cmporder utility to sort lines passed via stdin."""

import sys


def main() -> int:
    """Read lines from stdin, sort them, and write them to stdout."""
    lines = sys.stdin.read().splitlines()
    lines.sort()
    for line in lines:
        print(line)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
