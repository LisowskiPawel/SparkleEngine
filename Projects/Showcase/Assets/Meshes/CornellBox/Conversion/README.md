# Cornell Box Conversion

The source is `CornellBox.zip` from [Morgan McGuire's Computer Graphics Archive](https://casual-effects.com/g3d/data10/index.html). The archive's Cornell Box set was created by Guedis Cardenas and Morgan McGuire and is distributed under CC BY 3.0. `CornellBox-Original.obj` and its MTL additionally identify that original variant as public domain; Sparkle retains the archive-level attribution in [COPYRIGHT.txt](../COPYRIGHT.txt), tracked beside the GLB so it remains available without the external publisher cache.

**Use in Sparkle:** the retained CC BY 3.0 notice permits educational use, conversion and redistribution with attribution. Keep the authors, title, source link, [license link](https://creativecommons.org/licenses/by/3.0/) and modification description below with distributed source/cooked derivatives. Do not imply endorsement or apply additional restrictions to this asset. Sparkle's root MIT license does not replace this third-party notice.

**Attribution:** Cornell Box by Guedis Cardenas and Morgan McGuire, Williams College, 2011; obtained through Morgan McGuire's Computer Graphics Archive. Sparkle converts the original OBJ/MTL to GLB with triangulation, generated flat normals, Phong-to-roughness translation and emissive-strength mapping. The launcher thumbnail has its [own attribution](../../../../Launcher/Thumbnails/README.md).

`ConvertCornellBox.py` converts the original OBJ/MTL pair to one atomically published, tracked GLB 2.0 artifact. It validates the selected variant's source and material inventories, resolves OBJ indices, triangulates polygons in authored order, generates flat face normals, preserves diffuse colors and Phong roughness intent, and maps the ceiling panel emission through `KHR_materials_emissive_strength`. The other publisher variants stay in the external pack for future explicit levels; this level does not silently substitute them.

**Normal use:** the level uses the tracked, self-contained GLB and does not require the Publisher OBJ archive. Launcher selected-map sync therefore performs no Cornell download. The catalog retains the pinned Cornell archive recipe only for explicit source regeneration; the original server is currently unavailable from this workspace.

Run from the repository root after explicitly acquiring the Cornell Box publisher pack for regeneration:

```powershell
python Projects/Showcase/Assets/Meshes/CornellBox/Conversion/ConvertCornellBox.py
```

Expected conversion inventory:

- 8 materials and 8 material primitives;
- 72 face vertices and 36 triangles;
- `CornellBox.glb` SHA-256: `a2af112abeaa1b08087009afa3efef2c6b665c59bd7a6efe70d703e8f2349d91`.

The source archive is pinned in `Projects/Showcase/Levels.catalog` by byte count and SHA-256. Re-run the conversion only when that source identity or conversion policy changes, then update the output hashes above and validate the cooked scene before publishing the result.

**Verification (2026-10-10):** [ITER-CORNELL-01](../../../../../../Docs/Acceptance/ReleaseScope/Evidence.md#cornell-license-and-format-iter-cornell-01) reproduced the tracked GLB byte-for-byte and checked its container, inventory, finite vertex data, index bounds and embedded attribution. The retained original OBJ/MTL match an independently pinned mirror after CRLF normalization; this is not an exact ZIP receipt. The publisher ZIP still needs reacquisition against the unchanged catalog identity. Cooked rendering, single-emitter semantics and package notice inclusion remain separate checks.

[ITER-REL-01-06](../../../../../../Docs/Acceptance/ReleaseScope/Evidence.md#fr-01-exit-recheck-iter-rel-01-06) verifies selected-map sync from a checkout without `Publisher/`. The archive hash and GLB bytes are unchanged.
