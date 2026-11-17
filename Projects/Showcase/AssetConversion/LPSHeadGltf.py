"""Map the LPS Head mesh to its retained textured dielectric glTF scene."""

from __future__ import annotations

import math

from .Glb import GlbBuffer
from .LPSHeadMesh import HeadMesh


ASSET_METADATA = {
    "copyright": "Infinite Realities / Lee Perry-Smith, CC BY 3.0",
    "generator": "SparkleEngine deterministic LPS Head OBJ-to-glTF conversion",
    "version": "2.0",
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


def build_glb(mesh: HeadMesh) -> bytes:
    buffer = GlbBuffer()
    primitive = buffer.append_primitive(mesh.positions, mesh.normals, mesh.indices, 0, texcoords=mesh.texcoords)
    scene = build_scene(primitive)
    return buffer.encode(ASSET_METADATA, scene)
