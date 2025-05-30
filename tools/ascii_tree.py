#!/usr/bin/env python3
"""Print an ASCII tree representation of the repository."""

from __future__ import annotations

import os
from pathlib import Path
from typing import Iterable


def _walk(dir_path: Path, prefix: str = "") -> Iterable[str]:
    """Yield lines of an ASCII tree for the given directory."""
    entries = [
        p
        for p in dir_path.iterdir()
        if p.name not in {".git", "__pycache__", "build", "obj"}
    ]
    entries.sort(key=lambda p: p.name)
    for index, entry in enumerate(entries):
        connector = "└── " if index == len(entries) - 1 else "├── "
        yield f"{prefix}{connector}{entry.name}"
        if entry.is_dir():
            extension = "    " if index == len(entries) - 1 else "│   "
            yield from _walk(entry, prefix + extension)


def main() -> int:
    """Entry point for script."""
    root = Path(__file__).resolve().parents[1]
    for line in _walk(root):
        print(line)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
