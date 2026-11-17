"""Own glTF vertex/index storage and deterministic GLB 2.0 container encoding."""

from __future__ import annotations

import json
import math
import struct


FLOAT_COMPONENT = 5126
UNSIGNED_INT_COMPONENT = 5125
TRIANGLES_MODE = 4
GLB_MAGIC = 0x46546C67
GLB_VERSION = 2
GLB_JSON_CHUNK = 0x4E4F534A
GLB_BINARY_CHUNK = 0x004E4942


def vector_bounds(vectors: list[tuple[float, ...]]) -> tuple[list[float], list[float]]:
    component_count = len(vectors[0])
    minimum = [min(vector[component] for vector in vectors) for component in range(component_count)]
    maximum = [max(vector[component] for vector in vectors) for component in range(component_count)]
    return minimum, maximum


def _encode_glb(document: dict[str, object], binary: bytearray) -> bytes:
    json_chunk = json.dumps(document, separators=(",", ":"), ensure_ascii=True, allow_nan=False).encode("utf-8")
    json_chunk += b" " * ((-len(json_chunk)) % 4)
    binary_padding = b"\x00" * ((-len(binary)) % 4)
    binary_length = len(binary) + len(binary_padding)
    total_length = 12 + 8 + len(json_chunk) + 8 + binary_length
    return b"".join(
        (
            struct.pack("<III", GLB_MAGIC, GLB_VERSION, total_length),
            struct.pack("<II", len(json_chunk), GLB_JSON_CHUNK),
            json_chunk,
            struct.pack("<II", binary_length, GLB_BINARY_CHUNK),
            binary,
            binary_padding,
        )
    )


def _pack_vectors(vectors: list[tuple[float, ...]], component_count: int, vertex_count: int) -> bytes:
    if len(vectors) != vertex_count or any(len(vector) != component_count for vector in vectors):
        raise ValueError(f"Vertex stream must contain {vertex_count} vectors of {component_count} components.")
    if any(not math.isfinite(component) for vector in vectors for component in vector):
        raise ValueError("Vertex stream contains non-finite components.")

    flat_values = (component for vector in vectors for component in vector)
    try:
        return struct.pack(f"<{vertex_count * component_count}f", *flat_values)
    except OverflowError as error:
        raise ValueError("Vertex stream exceeds the float32 range.") from error


class GlbBuffer:
    """Keep each accessor, buffer view and binary range in the same build lifetime."""

    def __init__(self) -> None:
        self._binary = bytearray()
        self._buffer_views: list[dict[str, int]] = []
        self._accessors: list[dict[str, object]] = []

    def _append_accessor(
        self,
        payload: bytes,
        target: int,
        component_type: int,
        count: int,
        value_type: str,
        bounds: tuple[list[float], list[float]] | None = None,
    ) -> int:
        self._binary.extend(b"\x00" * ((-len(self._binary)) % 4))
        view_index = len(self._buffer_views)
        self._buffer_views.append({"buffer": 0, "byteLength": len(payload), "byteOffset": len(self._binary), "target": target})
        self._binary.extend(payload)

        accessor: dict[str, object] = {"bufferView": view_index, "componentType": component_type, "count": count}
        if bounds is not None:
            accessor["max"] = bounds[1]
            accessor["min"] = bounds[0]
        accessor["type"] = value_type
        accessor_index = len(self._accessors)
        self._accessors.append(accessor)
        return accessor_index

    def append_primitive(
        self,
        positions: list[tuple[float, float, float]],
        normals: list[tuple[float, float, float]],
        indices: list[int],
        material_index: int,
        *,
        texcoords: list[tuple[float, float]] | None = None,
    ) -> dict[str, object]:
        vertex_count = len(positions)
        if not vertex_count:
            raise ValueError("Primitive has no vertices.")
        if not indices or len(indices) % 3:
            raise ValueError("Primitive must contain complete triangles.")
        if any(index < 0 or index >= vertex_count for index in indices):
            raise ValueError("Primitive index is outside its vertex streams.")

        position_payload = _pack_vectors(positions, 3, vertex_count)
        normal_payload = _pack_vectors(normals, 3, vertex_count)
        texcoord_payload = _pack_vectors(texcoords, 2, vertex_count) if texcoords is not None else None
        index_payload = struct.pack(f"<{len(indices)}I", *indices)

        position_bounds = vector_bounds(positions)
        texcoord_bounds = vector_bounds(texcoords) if texcoords is not None else None
        position_accessor = self._append_accessor(position_payload, 34962, FLOAT_COMPONENT, vertex_count, "VEC3", position_bounds)
        normal_accessor = self._append_accessor(normal_payload, 34962, FLOAT_COMPONENT, vertex_count, "VEC3")
        attributes = {"NORMAL": normal_accessor, "POSITION": position_accessor}
        if texcoord_payload is not None:
            attributes["TEXCOORD_0"] = self._append_accessor(texcoord_payload, 34962, FLOAT_COMPONENT, vertex_count, "VEC2", texcoord_bounds)
        index_accessor = self._append_accessor(index_payload, 34963, UNSIGNED_INT_COMPONENT, len(indices), "SCALAR")
        return {"attributes": attributes, "indices": index_accessor, "material": material_index, "mode": TRIANGLES_MODE}

    def encode(self, asset_metadata: dict[str, str], scene: dict[str, object]) -> bytes:
        document = {
            "accessors": self._accessors,
            "asset": asset_metadata,
            "bufferViews": self._buffer_views,
            "buffers": [{"byteLength": len(self._binary)}],
            **scene,
        }
        return _encode_glb(document, self._binary)
