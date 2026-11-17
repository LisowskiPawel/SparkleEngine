"""Read the pinned LPS Head OBJ and enforce its source inventory and face syntax."""

from __future__ import annotations

from pathlib import Path

from .LPSHeadMesh import HeadMesh, HeadMeshBuilder
from .ObjGeometry import resolve_obj_index
from .ObjSource import parse_numbers, read_records


EXPECTED_SOURCE_POSITIONS = 8844
EXPECTED_SOURCE_TEXCOORDS = 35368
EXPECTED_SOURCE_FACES = 8842


def parse_face_vertex(token: str, counts: tuple[int, int], line_number: int) -> tuple[int, int]:
    fields = token.split("/")
    if len(fields) != 2 or any(not field for field in fields):
        raise ValueError(f"Face vertex '{token}' is missing position or texture-coordinate data at line {line_number}.")
    return tuple(resolve_obj_index(field, count, line_number) for field, count in zip(fields, counts, strict=True))


def parse_face(values: list[str], counts: tuple[int, int], active_material: str, line_number: int) -> list[tuple[int, int]]:
    if active_material != "defaultMat":
        raise ValueError(f"Face has no supported material at line {line_number}.")
    if len(values) < 3:
        raise ValueError(f"Face has fewer than three vertices at line {line_number}.")
    return [parse_face_vertex(token, counts, line_number) for token in values]


def validate_source_inventory(position_count: int, texcoord_count: int, face_count: int) -> None:
    source_inventory = (position_count, texcoord_count, face_count)
    expected_inventory = (EXPECTED_SOURCE_POSITIONS, EXPECTED_SOURCE_TEXCOORDS, EXPECTED_SOURCE_FACES)
    if source_inventory != expected_inventory:
        raise ValueError(f"Publisher OBJ inventory changed: expected {expected_inventory}, found {source_inventory}.")


def load_obj(source_path: Path) -> HeadMesh:
    source_positions: list[tuple[float, float, float]] = []
    source_texcoords: list[tuple[float, float]] = []
    builder: HeadMeshBuilder | None = None
    active_material = ""
    source_face_count = 0

    for line_number, keyword, values in read_records(source_path):
        if keyword in ("v", "vt") and builder is not None:
            raise ValueError(f"OBJ vertex streams must be declared before faces at line {line_number}.")

        if keyword == "v":
            source_positions.append(parse_numbers(values, 3, line_number))
        elif keyword == "vn":
            raise ValueError(f"Unexpected authored normal at line {line_number}; this conversion generates smooth normals from geometry.")
        elif keyword == "vt":
            u, v = parse_numbers(values, 2, line_number)
            source_texcoords.append((u, 1.0 - v))
        elif keyword == "usemtl":
            if len(values) != 1 or values[0] != "defaultMat":
                raise ValueError(f"Material declaration must select defaultMat at line {line_number}.")
            active_material = values[0]
        elif keyword == "f":
            counts = (len(source_positions), len(source_texcoords))
            source_indices = parse_face(values, counts, active_material, line_number)
            if builder is None:
                builder = HeadMeshBuilder(source_positions, source_texcoords)
            builder.append_face(source_indices)
            source_face_count += 1

    validate_source_inventory(len(source_positions), len(source_texcoords), source_face_count)
    if builder is None:
        raise ValueError("OBJ conversion found no faces.")
    return builder.finish()
