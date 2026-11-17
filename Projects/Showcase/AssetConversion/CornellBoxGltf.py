"""Map Cornell Box primitives and publisher materials to the retained glTF scene."""

from __future__ import annotations

import math

from .CornellBoxObj import Material, Primitive
from .Glb import GlbBuffer


ASSET_METADATA = {
    "copyright": "Guedis Cardenas and Morgan McGuire, CC BY 3.0 archive distribution",
    "generator": "SparkleEngine deterministic Cornell Box OBJ-to-glTF conversion",
    "version": "2.0",
}


def build_material(material: Material) -> dict[str, object]:
    roughness = math.sqrt(2.0 / (material.specular_exponent + 2.0))
    document: dict[str, object] = {
        "name": material.name,
        "pbrMetallicRoughness": {
            "baseColorFactor": [*material.base_color, 1.0],
            "metallicFactor": 0.0,
            "roughnessFactor": roughness,
        },
    }
    emissive_strength = max(material.emissive_color)
    if emissive_strength > 0.0:
        document["emissiveFactor"] = [component / emissive_strength for component in material.emissive_color]
        if emissive_strength > 1.0:
            document["extensions"] = {"KHR_materials_emissive_strength": {"emissiveStrength": emissive_strength}}
    return document


def build_scene(
    materials: dict[str, Material],
    mesh_primitives: list[dict[str, object]],
) -> dict[str, object]:
    return {
        "extensionsUsed": ["KHR_materials_emissive_strength"],
        "materials": [build_material(material) for material in materials.values()],
        "meshes": [{"name": "Cornell Box Original", "primitives": mesh_primitives}],
        "nodes": [{"mesh": 0, "name": "Cornell Box Original"}],
        "scene": 0,
        "scenes": [{"name": "Cornell Box Original", "nodes": [0]}],
    }


def build_glb(materials: dict[str, Material], primitives: dict[str, Primitive]) -> bytes:
    buffer = GlbBuffer()
    material_indices = {name: index for index, name in enumerate(materials)}
    mesh_primitives = [buffer.append_primitive(primitive.positions, primitive.normals, primitive.indices, material_indices[name]) for name, primitive in primitives.items()]
    scene = build_scene(materials, mesh_primitives)
    return buffer.encode(ASSET_METADATA, scene)
