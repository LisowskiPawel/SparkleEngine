"""Resolve the source/output request for an explicit Showcase asset conversion."""

from __future__ import annotations

import argparse
from pathlib import Path


def parse_arguments(asset_name: str, source_name: str, description: str) -> argparse.Namespace:
    asset_directory = Path(__file__).resolve().parents[1] / "Assets" / "Meshes" / asset_name
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument(
        "--source", type=Path, default=asset_directory / "Publisher" / source_name,
        help=f"Path to the publisher's {source_name} file.",
    )
    parser.add_argument(
        "--output", type=Path, default=asset_directory / f"{asset_name}.glb",
        help="Path for the generated GLB artifact.",
    )
    arguments = parser.parse_args()
    arguments.source = arguments.source.resolve()
    arguments.output = arguments.output.resolve()
    if arguments.output.suffix.lower() != ".glb":
        raise ValueError(f"Output must be one transactionally published .glb artifact: {arguments.output}")
    return arguments
