"""Deterministically convert the publisher's LPS Head OBJ geometry to glTF 2.0."""

from .CommandLine import parse_arguments
from .LPSHeadObj import load_obj, summarize_mesh
from .LPSHeadGltf import build_glb
from .Publication import publish_conversion


def main() -> None:
    arguments = parse_arguments("LPSHead", "head.OBJ", __doc__)
    mesh = load_obj(arguments.source)
    glb = build_glb(mesh)
    publish_conversion(arguments.output, glb, summarize_mesh(mesh))


if __name__ == "__main__":
    main()
