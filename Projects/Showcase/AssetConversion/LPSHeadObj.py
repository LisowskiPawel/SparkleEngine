"""Parse the pinned LPS Head OBJ and build its smooth, UV-seamed mesh."""

from __future__ import annotations

import math
from dataclasses import dataclass
from pathlib import Path

from .ObjGeometry import polygon_normal, resolve_obj_index


EXPECTED_SOURCE_POSITIONS = 8844
EXPECTED_SOURCE_TEXCOORDS = 35368
EXPECTED_SOURCE_FACES = 8842
MINIMUM_NORMAL_LENGTH_SQUARED = 1.0e-16


@dataclass
class ConvertedMesh:
    positions: list[tuple[float, float, float]]
    normals: list[tuple[float, float, float]]
    texcoords: list[tuple[float, float]]
    indices: list[int]
    skipped_degenerate_faces: int


def parse_face_vertex(token: str, counts: tuple[int, int], line_number: int) -> tuple[int, int]:
    fields = token.split("/")
    if len(fields) != 2 or any(not field for field in fields):
        raise ValueError(f"Face vertex '{token}' is missing position or texture-coordinate data at line {line_number}.")
    return tuple(resolve_obj_index(field, count, line_number) for field, count in zip(fields, counts, strict=True))


def normalized(value: tuple[float, float, float], context: str) -> tuple[float, float, float]:
    length = math.sqrt(sum(component * component for component in value))
    if not math.isfinite(length) or length <= 1.0e-8:
        raise ValueError(f"Normal has no usable direction for {context}.")
    return tuple(component / length for component in value)


class _MeshBuilder:
    """Own vertex interning and source-position normal accumulation for one OBJ read."""

    def __init__(self) -> None:
        self.positions: list[tuple[float, float, float]] = []
        self.texcoords: list[tuple[float, float]] = []
        self.indices: list[int] = []
        self.expanded_source_positions: list[int] = []
        self.vertex_indices: dict[tuple[int, float, float], int] = {}
        self.accumulated_normals: list[list[float]] = []
        self.skipped_degenerate_faces = 0

    def _prepare_normal_accumulators(self, position_count: int) -> None:
        if not self.accumulated_normals:
            self.accumulated_normals = [[0.0, 0.0, 0.0] for _ in range(position_count)]
        if len(self.accumulated_normals) != position_count:
            raise ValueError("OBJ positions must be declared before faces.")

    def _expand_face(
        self, source_indices: list[tuple[int, int]], source_positions: list[tuple[float, float, float]],
        source_texcoords: list[tuple[float, float]],
    ) -> list[int]:
        face: list[int] = []
        for position_index, texcoord_index in source_indices:
            texcoord = source_texcoords[texcoord_index]
            vertex_key = (position_index, texcoord[0], texcoord[1])
            vertex_index = self.vertex_indices.get(vertex_key)
            if vertex_index is None:
                vertex_index = len(self.positions)
                self.vertex_indices[vertex_key] = vertex_index
                self.positions.append(source_positions[position_index])
                self.texcoords.append(texcoord)
                self.expanded_source_positions.append(position_index)
            face.append(vertex_index)
        return face

    def _accumulate_normal(self, position_indices: list[int], normal: list[float]) -> None:
        for position_index in position_indices:
            accumulator = self.accumulated_normals[position_index]
            for component in range(3):
                accumulator[component] += normal[component]

    def _triangulate_face(self, face: list[int]) -> None:
        for offset in range(1, len(face) - 1):
            self.indices.extend((face[0], face[offset], face[offset + 1]))

    def append_face(
        self, source_indices: list[tuple[int, int]], source_positions: list[tuple[float, float, float]],
        source_texcoords: list[tuple[float, float]],
    ) -> None:
        position_indices = [source_index[0] for source_index in source_indices]
        self._prepare_normal_accumulators(len(source_positions))
        normal = polygon_normal(source_positions[index] for index in position_indices)
        if sum(component * component for component in normal) <= MINIMUM_NORMAL_LENGTH_SQUARED:
            self.skipped_degenerate_faces += 1
            return

        face = self._expand_face(source_indices, source_positions, source_texcoords)
        self._accumulate_normal(position_indices, normal)
        self._triangulate_face(face)

    def finish(self) -> ConvertedMesh:
        if not self.positions or len(self.positions) != len(self.texcoords):
            raise ValueError("OBJ conversion produced incomplete vertex streams.")
        if not self.indices or len(self.indices) % 3:
            raise ValueError("OBJ conversion produced incomplete triangle indices.")

        normals = [
            normalized(tuple(self.accumulated_normals[position_index]), f"source position {position_index + 1}")
            for position_index in self.expanded_source_positions
        ]
        return ConvertedMesh(self.positions, normals, self.texcoords, self.indices, self.skipped_degenerate_faces)


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


def load_obj(source_path: Path) -> ConvertedMesh:
    if not source_path.is_file():
        raise FileNotFoundError(f"Publisher OBJ was not found: {source_path}")

    source_positions: list[tuple[float, float, float]] = []
    source_texcoords: list[tuple[float, float]] = []
    builder = _MeshBuilder()
    active_material = ""
    source_face_count = 0

    for line_number, raw_line in enumerate(source_path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue

        fields = line.split()
        keyword = fields[0]
        values = fields[1:]
        if keyword == "v":
            if len(values) < 3:
                raise ValueError(f"Position is incomplete at line {line_number}.")
            source_positions.append(tuple(float(value) for value in values[:3]))
        elif keyword == "vn":
            raise ValueError(f"Unexpected authored normal at line {line_number}; this conversion generates smooth normals from geometry.")
        elif keyword == "vt":
            if len(values) < 2:
                raise ValueError(f"Texture coordinate is incomplete at line {line_number}.")
            source_texcoords.append((float(values[0]), 1.0 - float(values[1])))
        elif keyword == "usemtl":
            if len(values) != 1:
                raise ValueError(f"Material declaration is malformed at line {line_number}.")
            active_material = values[0]
            if active_material != "defaultMat":
                raise ValueError(f"Unexpected material '{active_material}' at line {line_number}.")
        elif keyword == "f":
            source_face_count += 1
            counts = (len(source_positions), len(source_texcoords))
            source_indices = parse_face(values, counts, active_material, line_number)
            builder.append_face(source_indices, source_positions, source_texcoords)

    validate_source_inventory(len(source_positions), len(source_texcoords), source_face_count)
    return builder.finish()


def summarize_mesh(mesh: ConvertedMesh) -> str:
    return (
        f"vertices={len(mesh.positions)} triangles={len(mesh.indices) // 3} "
        f"skipped_degenerate_faces={mesh.skipped_degenerate_faces}"
    )
