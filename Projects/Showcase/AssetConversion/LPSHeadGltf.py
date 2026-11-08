"""Map the LPS Head mesh to its retained textured dielectric glTF scene."""

from __future__ import annotations

import math

from .Glb import GlbBuffer, TRIANGLES_MODE
from .LPSHeadObj import ConvertedMesh


ASSET_METADATA = {
    "copyright": "Infinite Realities / Lee Perry-Smith, CC BY 3.0",
    "generator": "SparkleEngine deterministic LPS Head OBJ-to-glTF conversion",
    "version": "2.0",
}


def append_primitive(buffer: GlbBuffer, mesh: ConvertedMesh) -> dict[str, object]:
    position_accessor = buffer.append_vectors(mesh.positions, include_bounds=True)
    normal_accessor = buffer.append_vectors(mesh.normals)
    texcoord_accessor = buffer.append_vectors(mesh.texcoords, include_bounds=True)
    index_accessor = buffer.append_indices(mesh.indices)
    return {
        "attributes": {"NORMAL": normal_accessor, "POSITION": position_accessor, "TEXCOORD_0": texcoord_accessor},
        "indices": index_accessor,
        "material": 0,
        "mode": TRIANGLES_MODE,
    }


def build_scene(primitive: dict[str, object]) -> dict[str, object]:
    return {
        "images": [{"uri": "Publisher/lambertian.jpg"}],
        "materials": [
            {
                "name": "LPS Head Skin Baseline",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {"index": 0},
                    "metallicFactor": 0.0,
                    "roughnessFactor": math.sqrt(2.0 / 7.0),
                },
            }
        ],
        "meshes": [{"name": "LPS Head", "primitives": [primitive]}],
        "nodes": [{"mesh": 0, "name": "LPS Head"}],
        "samplers": [{"magFilter": 9729, "minFilter": 9987, "wrapS": 10497, "wrapT": 10497}],
        "scene": 0,
        "scenes": [{"name": "LPS Head", "nodes": [0]}],
        "textures": [{"sampler": 0, "source": 0}],
    }


def build_glb(mesh: ConvertedMesh) -> bytes:
    buffer = GlbBuffer()
    primitive = append_primitive(buffer, mesh)
    scene = build_scene(primitive)
    return buffer.encode(ASSET_METADATA, scene)
