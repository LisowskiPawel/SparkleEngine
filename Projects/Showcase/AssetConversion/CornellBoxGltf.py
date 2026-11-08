"""Map Cornell Box primitives and publisher materials to the retained glTF scene."""

from __future__ import annotations

import math
from collections import OrderedDict

from .CornellBoxObj import Material, Primitive
from .Glb import GlbBuffer, TRIANGLES_MODE


ASSET_METADATA = {
    "copyright": "Guedis Cardenas and Morgan McGuire, CC BY 3.0 archive distribution",
    "generator": "SparkleEngine deterministic Cornell Box OBJ-to-glTF conversion",
    "version": "2.0",
}


def build_material(material: Material) -> dict[str, object]:
    roughness = math.sqrt(2.0 / (max(material.specular_exponent, 0.0) + 2.0))
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


def append_primitive(buffer: GlbBuffer, primitive: Primitive, material_index: int) -> dict[str, object]:
    position_accessor = buffer.append_vectors(primitive.positions, include_bounds=True)
    normal_accessor = buffer.append_vectors(primitive.normals)
    index_accessor = buffer.append_indices(primitive.indices)
    return {
        "attributes": {"NORMAL": normal_accessor, "POSITION": position_accessor},
        "indices": index_accessor,
        "material": material_index,
        "mode": TRIANGLES_MODE,
    }


def build_scene(
    materials: OrderedDict[str, Material], mesh_primitives: list[dict[str, object]],
) -> dict[str, object]:
    return {
        "extensionsUsed": ["KHR_materials_emissive_strength"],
        "materials": [build_material(material) for material in materials.values()],
        "meshes": [{"name": "Cornell Box Original", "primitives": mesh_primitives}],
        "nodes": [{"mesh": 0, "name": "Cornell Box Original"}],
        "scene": 0,
        "scenes": [{"name": "Cornell Box Original", "nodes": [0]}],
    }


def append_primitives(
    buffer: GlbBuffer, materials: OrderedDict[str, Material], primitives: OrderedDict[str, Primitive],
) -> list[dict[str, object]]:
    material_indices = {name: index for index, name in enumerate(materials)}
    return [append_primitive(buffer, primitive, material_indices[name]) for name, primitive in primitives.items()]


def build_glb(materials: OrderedDict[str, Material], primitives: OrderedDict[str, Primitive]) -> bytes:
    buffer = GlbBuffer()
    mesh_primitives = append_primitives(buffer, materials, primitives)
    scene = build_scene(materials, mesh_primitives)
    return buffer.encode(ASSET_METADATA, scene)
