"""Parse the pinned Cornell Box OBJ/MTL pair into flat-shaded primitives."""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path

from .ObjGeometry import normalized, polygon_normal, resolve_obj_index
from .ObjSource import parse_numbers, read_records


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


def load_materials(material_path: Path) -> dict[str, Material]:
    materials: dict[str, Material] = {}
    current: Material | None = None
    for line_number, keyword, values in read_records(material_path):
        if keyword == "newmtl":
            if len(values) != 1 or values[0] in materials:
                raise ValueError(f"Material declaration is malformed or duplicated at line {line_number}.")
            current = Material(values[0])
            materials[current.name] = current
        elif keyword in ("Kd", "Ke", "Ns"):
            if current is None:
                raise ValueError(f"Material property precedes its declaration at line {line_number}.")
            if keyword == "Kd":
                current.base_color = parse_numbers(values, 3, line_number)
                if any(component < 0.0 or component > 1.0 for component in current.base_color):
                    raise ValueError(f"Diffuse color is outside [0, 1] at line {line_number}.")
            elif keyword == "Ke":
                current.emissive_color = parse_numbers(values, 3, line_number)
                if any(component < 0.0 for component in current.emissive_color):
                    raise ValueError(f"Emissive color is negative at line {line_number}.")
            else:
                current.specular_exponent = parse_numbers(values, 1, line_number)[0]
                if current.specular_exponent < 0.0:
                    raise ValueError(f"Specular exponent is negative at line {line_number}.")
    if tuple(materials) != EXPECTED_MATERIAL_NAMES:
        raise ValueError(f"Publisher MTL inventory changed: found {tuple(materials)}.")
    return materials


def append_face(
    primitive: Primitive,
    values: list[str],
    source_positions: list[tuple[float, float, float]],
    line_number: int,
) -> None:
    source_indices = [resolve_obj_index(value, len(source_positions), line_number) for value in values]
    vertices = [source_positions[index] for index in source_indices]
    normal = normalized(polygon_normal(vertices), f"face at line {line_number}")
    first_vertex = len(primitive.positions)
    primitive.positions.extend(vertices)
    primitive.normals.extend([normal] * len(vertices))
    for offset in range(1, len(vertices) - 1):
        primitive.indices.extend((first_vertex, first_vertex + offset, first_vertex + offset + 1))


def validate_source_inventory(
    source_path: Path,
    declared_material_library: str,
    position_count: int,
    face_count: int,
    primitives: dict[str, Primitive],
) -> None:
    if declared_material_library != source_path.with_suffix(".mtl").name:
        raise ValueError("OBJ material-library identity does not match the selected source variant.")
    source_inventory = (position_count, face_count)
    expected_inventory = (EXPECTED_SOURCE_POSITIONS, EXPECTED_SOURCE_FACES)
    if source_inventory != expected_inventory:
        raise ValueError(f"Publisher OBJ inventory changed: expected {expected_inventory}, found {source_inventory}.")
    if tuple(primitives) != EXPECTED_PRIMITIVE_NAMES:
        raise ValueError(f"Publisher OBJ material usage changed: found {tuple(primitives)}.")
    if any(not primitive.indices for primitive in primitives.values()):
        raise ValueError("OBJ conversion produced incomplete material primitives.")


def load_obj(source_path: Path, materials: dict[str, Material]) -> dict[str, Primitive]:
    source_positions: list[tuple[float, float, float]] = []
    primitives: dict[str, Primitive] = {}
    active_material = ""
    declared_material_library = ""
    source_face_count = 0

    for line_number, keyword, values in read_records(source_path):
        if keyword == "mtllib":
            if len(values) != 1 or declared_material_library:
                raise ValueError(f"Material library declaration is malformed at line {line_number}.")
            declared_material_library = values[0]
        elif keyword == "v":
            source_positions.append(parse_numbers(values, 3, line_number))
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


def load_source(source_path: Path) -> tuple[dict[str, Material], dict[str, Primitive]]:
    material_path = source_path.with_suffix(".mtl")
    if not source_path.is_file() or not material_path.is_file():
        raise FileNotFoundError(f"Publisher OBJ/MTL pair was not found: {source_path}")

    materials = load_materials(material_path)
    primitives = load_obj(source_path, materials)
    return materials, primitives


def summarize_source(materials: dict[str, Material], primitives: dict[str, Primitive]) -> str:
    vertex_count = sum(len(primitive.positions) for primitive in primitives.values())
    triangle_count = sum(len(primitive.indices) // 3 for primitive in primitives.values())
    return f"materials={len(materials)} primitives={len(primitives)} vertices={vertex_count} triangles={triangle_count}"
