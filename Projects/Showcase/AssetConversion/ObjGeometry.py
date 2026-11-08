"""Own OBJ index resolution and the shared unnormalized polygon-normal algorithm."""

from __future__ import annotations

from collections.abc import Iterable
from itertools import chain


def resolve_obj_index(value: str, count: int, line_number: int) -> int:
    index = int(value)
    resolved = index - 1 if index > 0 else count + index
    if resolved < 0 or resolved >= count:
        raise ValueError(f"OBJ index {index} is out of range at line {line_number}.")
    return resolved


def polygon_normal(vertices: Iterable[tuple[float, float, float]]) -> list[float]:
    normal = [0.0, 0.0, 0.0]
    remaining = iter(vertices)
    first = next(remaining)
    current = first
    for following in chain(remaining, (first,)):
        normal[0] += (current[1] - following[1]) * (current[2] + following[2])
        normal[1] += (current[2] - following[2]) * (current[0] + following[0])
        normal[2] += (current[0] - following[0]) * (current[1] + following[1])
        current = following
    return normal
