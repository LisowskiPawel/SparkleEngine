"""Parse the pinned Cornell Box OBJ/MTL pair into flat-shaded primitives."""

from __future__ import annotations

import math
from collections import OrderedDict
from dataclasses import dataclass, field
from pathlib import Path

from .ObjGeometry import polygon_normal, resolve_obj_index


EXPECTED_MATERIAL_NAMES = (
    "leftWall",
    "rightWall",
    "floor",
    "ceiling",
    "backWall",
    "shortBox",
    "tallBox",
    "light",
)
EXPECTED_PRIMITIVE_NAMES = (
    "floor",
    "ceiling",
    "backWall",
    "rightWall",
    "leftWall",
    "shortBox",
    "tallBox",
    "light",
)
EXPECTED_SOURCE_POSITIONS = 72
EXPECTED_SOURCE_FACES = 18


@dataclass
class Material:
    name: str
    base_color: tuple[float, float, float] = (1.0, 1.0, 1.0)
    emissive_color: tuple[float, float, float] = (0.0, 0.0, 0.0)
    specular_exponent: float = 10.0


@dataclass
class Primitive:
    positions: list[tuple[float, float, float]] = field(default_factory=list)
    normals: list[tuple[float, float, float]] = field(default_factory=list)
    indices: list[int] = field(default_factory=list)


def numeric_fields(line: str) -> list[float]:
    return [float(value) for value in line.split("#", 1)[0].split()[1:]]


def load_materials(material_path: Path) -> OrderedDict[str, Material]:
    materials: OrderedDict[str, Material] = OrderedDict()
    current: Material | None = None
    for line_number, raw_line in enumerate(material_path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        keyword = line.split(maxsplit=1)[0]
        if keyword == "newmtl":
            fields = line.split()
            if len(fields) != 2 or fields[1] in materials:
                raise ValueError(f"Material declaration is malformed or duplicated at line {line_number}.")
            current = Material(fields[1])
            materials[current.name] = current
        elif current is not None and keyword == "Kd":
            values = numeric_fields(line)
            if len(values) != 3:
                raise ValueError(f"Diffuse color is malformed at line {line_number}.")
            current.base_color = tuple(values)
        elif current is not None and keyword == "Ke":
            values = numeric_fields(line)
            if len(values) != 3:
                raise ValueError(f"Emissive color is malformed at line {line_number}.")
            current.emissive_color = tuple(values)
        elif current is not None and keyword == "Ns":
            values = numeric_fields(line)
            if len(values) != 1:
                raise ValueError(f"Specular exponent is malformed at line {line_number}.")
            current.specular_exponent = values[0]
    if not materials:
        raise ValueError("MTL conversion found no materials.")
    if tuple(materials) != EXPECTED_MATERIAL_NAMES:
        raise ValueError(f"Publisher MTL inventory changed: found {tuple(materials)}.")
    return materials


def face_normal(vertices: list[tuple[float, float, float]], line_number: int) -> tuple[float, float, float]:
    normal = polygon_normal(vertices)
    length = math.sqrt(sum(component * component for component in normal))
    if not math.isfinite(length) or length <= 1.0e-8:
        raise ValueError(f"Face has no usable normal at line {line_number}.")
    return tuple(component / length for component in normal)


def append_face(
    primitive: Primitive, values: list[str], source_positions: list[tuple[float, float, float]], line_number: int,
) -> None:
    source_indices = [resolve_obj_index(value, len(source_positions), line_number) for value in values]
    vertices = [source_positions[index] for index in source_indices]
    normal = face_normal(vertices, line_number)
    first_vertex = len(primitive.positions)
    primitive.positions.extend(vertices)
    primitive.normals.extend([normal] * len(vertices))
    for offset in range(1, len(vertices) - 1):
        primitive.indices.extend((first_vertex, first_vertex + offset, first_vertex + offset + 1))


def validate_source_inventory(
    source_path: Path, declared_material_library: str, position_count: int, face_count: int,
    primitives: OrderedDict[str, Primitive],
) -> None:
    if declared_material_library != source_path.with_suffix(".mtl").name:
        raise ValueError("OBJ material-library identity does not match the selected source variant.")
    source_inventory = (position_count, face_count)
    expected_inventory = (EXPECTED_SOURCE_POSITIONS, EXPECTED_SOURCE_FACES)
    if source_inventory != expected_inventory:
        raise ValueError(f"Publisher OBJ inventory changed: expected {expected_inventory}, found {source_inventory}.")
    if tuple(primitives) != EXPECTED_PRIMITIVE_NAMES:
        raise ValueError(f"Publisher OBJ material usage changed: found {tuple(primitives)}.")
    if not primitives or any(not primitive.indices for primitive in primitives.values()):
        raise ValueError("OBJ conversion produced incomplete material primitives.")


def load_obj(source_path: Path, materials: OrderedDict[str, Material]) -> OrderedDict[str, Primitive]:
    source_positions: list[tuple[float, float, float]] = []
    primitives: OrderedDict[str, Primitive] = OrderedDict()
    active_material = ""
    declared_material_library = ""
    source_face_count = 0

    for line_number, raw_line in enumerate(source_path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        fields = line.split()
        keyword = fields[0]
        values = fields[1:]
        if keyword == "mtllib":
            if len(values) != 1:
                raise ValueError(f"Material library declaration is malformed at line {line_number}.")
            declared_material_library = values[0]
        elif keyword == "v":
            if len(values) != 3:
                raise ValueError(f"Position is incomplete at line {line_number}.")
            source_positions.append(tuple(float(value) for value in values))
        elif keyword == "usemtl":
            if len(values) != 1 or values[0] not in materials:
                raise ValueError(f"Material reference is malformed or unknown at line {line_number}.")
            active_material = values[0]
            primitives.setdefault(active_material, Primitive())
        elif keyword == "f":
            source_face_count += 1
            if active_material not in primitives or len(values) < 3:
                raise ValueError(f"Face has no supported material or too few vertices at line {line_number}.")
            append_face(primitives[active_material], values, source_positions, line_number)

    validate_source_inventory(source_path, declared_material_library, len(source_positions), source_face_count, primitives)
    return primitives


def load_source(source_path: Path) -> tuple[OrderedDict[str, Material], OrderedDict[str, Primitive]]:
    material_path = source_path.with_suffix(".mtl")
    if not source_path.is_file() or not material_path.is_file():
        raise FileNotFoundError(f"Publisher OBJ/MTL pair was not found: {source_path}")

    materials = load_materials(material_path)
    primitives = load_obj(source_path, materials)
    return materials, primitives


def summarize_source(materials: OrderedDict[str, Material], primitives: OrderedDict[str, Primitive]) -> str:
    vertex_count = sum(len(primitive.positions) for primitive in primitives.values())
    triangle_count = sum(len(primitive.indices) // 3 for primitive in primitives.values())
    return f"materials={len(materials)} primitives={len(primitives)} vertices={vertex_count} triangles={triangle_count}"
