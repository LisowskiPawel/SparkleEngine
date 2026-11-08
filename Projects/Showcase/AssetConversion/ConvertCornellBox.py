"""Deterministically convert the publisher's original Cornell Box OBJ to glTF 2.0."""

from .CommandLine import parse_arguments
from .CornellBoxObj import load_source, summarize_source
from .CornellBoxGltf import build_glb
from .Publication import publish_conversion


def main() -> None:
    arguments = parse_arguments("CornellBox", "CornellBox-Original.obj", __doc__)
    materials, primitives = load_source(arguments.source)
    glb = build_glb(materials, primitives)
    publish_conversion(arguments.output, glb, summarize_source(materials, primitives))


if __name__ == "__main__":
    main()
