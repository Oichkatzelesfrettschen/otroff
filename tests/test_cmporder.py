"""Tests for the cmporder utility."""

from __future__ import annotations

import random
import subprocess
import sys
from pathlib import Path


def run_cmporder(input_lines: list[str]) -> list[str]:
    """Run cmporder with the given input lines and return output lines."""
    cmporder_path = Path(__file__).resolve().parents[1] / "tools" / "cmporder.py"
    joined = "\n".join(input_lines) + "\n"
    result = subprocess.run(
        [sys.executable, str(cmporder_path)], input=joined.encode(), capture_output=True, check=True
    )
    return result.stdout.decode().splitlines()


def test_unsorted_input_sorted_output() -> None:
    """Ensure cmporder sorts unsorted input."""
    numbers = [str(random.randint(0, 9999)) for _ in range(20)]
    expected = sorted(numbers)
    output = run_cmporder(numbers)
    assert output == expected

