"""Tests for the cmporder utility."""

from __future__ import annotations

import random
import subprocess
import sys
from pathlib import Path


def run_cmporder(input_lines: list[str]) -> list[str]:
    """Run cmporder with the given input lines and return output lines."""

    # Path to the cmporder executable script.
    cmporder_path = Path(__file__).resolve().parents[1] / "tools" / "cmporder.py"

    # Create a single string from the input lines terminated by a newline.
    joined = "\n".join(input_lines) + "\n"

    # Execute the cmporder script using the current Python interpreter.
    result = subprocess.run(
        [sys.executable, str(cmporder_path)],
        input=joined.encode(),
        capture_output=True,
        check=True,
    )

    # Split the output back into separate lines.
    return result.stdout.decode().splitlines()


def test_unsorted_input_sorted_output() -> None:
    """Ensure cmporder sorts unsorted input."""
    numbers = [str(random.randint(0, 9999)) for _ in range(20)]
    expected = sorted(numbers)
    output = run_cmporder(numbers)
    assert output == expected


def test_duplicate_lines_preserved() -> None:
    """Ensure cmporder retains duplicate lines while sorting."""

    # Input list containing duplicates in random order.
    lines = ["3", "1", "3", "2", "2"]

    # Expected result is the sorted list with duplicates preserved.
    expected = sorted(lines)

    # Run cmporder and verify the output matches the expected list.
    output = run_cmporder(lines)
    assert output == expected


def test_sorted_input_unchanged() -> None:
    """Verify sorted input comes out unchanged."""

    # Already sorted list of numbers.
    lines = ["1", "2", "3", "4"]

    # cmporder should not alter the ordering of an already sorted list.
    output = run_cmporder(lines)
    assert output == lines
