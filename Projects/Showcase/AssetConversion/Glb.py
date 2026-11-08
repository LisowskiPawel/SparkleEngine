"""Own glTF vertex/index storage and deterministic GLB 2.0 container encoding."""

from __future__ import annotations

import json
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
    json_chunk = json.dumps(document, separators=(",", ":"), ensure_ascii=True).encode("utf-8")
    json_chunk += b" " * ((-len(json_chunk)) % 4)
    binary.extend(b"\x00" * ((-len(binary)) % 4))
    total_length = 12 + 8 + len(json_chunk) + 8 + len(binary)
    return b"".join(
        (
            struct.pack("<III", GLB_MAGIC, GLB_VERSION, total_length),
            struct.pack("<II", len(json_chunk), GLB_JSON_CHUNK),
            json_chunk,
            struct.pack("<II", len(binary), GLB_BINARY_CHUNK),
            binary,
        )
    )


class GlbBuffer:
    """Keep each accessor, buffer view and binary range in the same build lifetime."""

    def __init__(self) -> None:
        self._binary = bytearray()
        self._buffer_views: list[dict[str, int]] = []
        self._accessors: list[dict[str, object]] = []

    def _append_accessor(
        self, payload: bytes, target: int, component_type: int, count: int, value_type: str,
        bounds: tuple[list[float], list[float]] | None = None,
    ) -> int:
        self._binary.extend(b"\x00" * ((-len(self._binary)) % 4))
        view_index = len(self._buffer_views)
        self._buffer_views.append(
            {"buffer": 0, "byteLength": len(payload), "byteOffset": len(self._binary), "target": target}
        )
        self._binary.extend(payload)

        accessor: dict[str, object] = {"bufferView": view_index, "componentType": component_type, "count": count}
        if bounds is not None:
            accessor["max"] = bounds[1]
            accessor["min"] = bounds[0]
        accessor["type"] = value_type
        accessor_index = len(self._accessors)
        self._accessors.append(accessor)
        return accessor_index

    def append_vectors(self, vectors: list[tuple[float, ...]], *, include_bounds: bool = False) -> int:
        component_count = len(vectors[0])
        flat_values = (component for vector in vectors for component in vector)
        payload = struct.pack(f"<{sum(len(vector) for vector in vectors)}f", *flat_values)
        bounds = vector_bounds(vectors) if include_bounds else None
        return self._append_accessor(payload, 34962, FLOAT_COMPONENT, len(vectors), f"VEC{component_count}", bounds)

    def append_indices(self, indices: list[int]) -> int:
        payload = struct.pack(f"<{len(indices)}I", *indices)
        return self._append_accessor(payload, 34963, UNSIGNED_INT_COMPONENT, len(indices), "SCALAR")

    def encode(self, asset_metadata: dict[str, str], scene: dict[str, object]) -> bytes:
        document = {
            "accessors": self._accessors,
            "asset": asset_metadata,
            "bufferViews": self._buffer_views,
            "buffers": [{"byteLength": len(self._binary)}],
            **scene,
        }
        return _encode_glb(document, self._binary)
