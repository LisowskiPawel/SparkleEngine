#pragma once

#include "Renderer/Public/Meshes/MeshDiagnostics.h"
#include "Renderer/Public/Diagnostics/MeshPreviewGeometry.h"

class GpuMeshCache;
class Mesh;
class RenderScene;

MeshDiagnosticsSnapshot CollectMeshDiagnostics(const RenderScene& scene, const GpuMeshCache* gpuMeshCache);
MeshPreviewGeometry CollectMeshPreviewGeometry(const RenderScene& scene, std::uintptr_t meshRuntimeId);
