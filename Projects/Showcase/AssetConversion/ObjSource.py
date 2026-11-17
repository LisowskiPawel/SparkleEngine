"""Stream OBJ/MTL records and validate numeric fields at the source boundary."""

from __future__ import annotations

import math
from collections.abc import Iterator
from pathlib import Path


def read_records(source_path: Path) -> Iterator[tuple[int, str, list[str]]]:
    with source_path.open(encoding="utf-8") as source:
        for line_number, line in enumerate(source, start=1):
            fields = line.partition("#")[0].split()
            if fields:
                yield line_number, fields[0], fields[1:]


def parse_numbers(values: list[str], count: int, line_number: int) -> tuple[float, ...]:
    if len(values) != count:
        raise ValueError(f"Expected {count} numeric fields at line {line_number}, found {len(values)}.")

    try:
        numbers = tuple(float(value) for value in values)
    except ValueError as error:
        raise ValueError(f"Malformed numeric field at line {line_number}.") from error

    if not all(math.isfinite(number) for number in numbers):
        raise ValueError(f"Non-finite numeric field at line {line_number}.")
    return numbers
