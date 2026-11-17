"""Build the LPS Head's UV-seamed mesh with source-position smooth normals."""

from __future__ import annotations

from dataclasses import dataclass

from .ObjGeometry import normalized, polygon_normal


MINIMUM_NORMAL_LENGTH_SQUARED = 1.0e-16


@dataclass
class HeadMesh:
    positions: list[tuple[float, float, float]]
    normals: list[tuple[float, float, float]]
    texcoords: list[tuple[float, float]]
    indices: list[int]
    skipped_degenerate_faces: int

    def summary(self) -> str:
        return f"vertices={len(self.positions)} triangles={len(self.indices) // 3} skipped_degenerate_faces={self.skipped_degenerate_faces}"


class HeadMeshBuilder:
    """Borrow complete source streams and own interning and normal accumulation."""

    def __init__(
        self,
        source_positions: list[tuple[float, float, float]],
        source_texcoords: list[tuple[float, float]],
    ) -> None:
        self._source_positions = source_positions
        self._source_texcoords = source_texcoords
        self._positions: list[tuple[float, float, float]] = []
        self._texcoords: list[tuple[float, float]] = []
        self._indices: list[int] = []
        self._expanded_source_positions: list[int] = []
        self._vertex_indices: dict[tuple[int, float, float], int] = {}
        self._accumulated_normals = [[0.0, 0.0, 0.0] for _ in source_positions]
        self._skipped_degenerate_faces = 0

    def _expand_face(self, source_indices: list[tuple[int, int]]) -> list[int]:
        face: list[int] = []
        for position_index, texcoord_index in source_indices:
            texcoord = self._source_texcoords[texcoord_index]
            vertex_key = (position_index, texcoord[0], texcoord[1])
            vertex_index = self._vertex_indices.get(vertex_key)
            if vertex_index is None:
                vertex_index = len(self._positions)
                self._vertex_indices[vertex_key] = vertex_index
                self._positions.append(self._source_positions[position_index])
                self._texcoords.append(texcoord)
                self._expanded_source_positions.append(position_index)
            face.append(vertex_index)
        return face

    def append_face(self, source_indices: list[tuple[int, int]]) -> None:
        position_indices = [source_index[0] for source_index in source_indices]
        normal = polygon_normal(self._source_positions[index] for index in position_indices)
        if sum(component * component for component in normal) <= MINIMUM_NORMAL_LENGTH_SQUARED:
            self._skipped_degenerate_faces += 1
            return

        face = self._expand_face(source_indices)
        for position_index in position_indices:
            accumulator = self._accumulated_normals[position_index]
            for component in range(3):
                accumulator[component] += normal[component]

        for offset in range(1, len(face) - 1):
            self._indices.extend((face[0], face[offset], face[offset + 1]))

    def finish(self) -> HeadMesh:
        if not self._indices:
            raise ValueError("OBJ conversion produced no triangles.")

        normals = [normalized(self._accumulated_normals[position_index], f"source position {position_index + 1}") for position_index in self._expanded_source_positions]
        return HeadMesh(self._positions, normals, self._texcoords, self._indices, self._skipped_degenerate_faces)
