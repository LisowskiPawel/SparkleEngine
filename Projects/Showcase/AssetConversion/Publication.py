"""Publish one complete conversion artifact and its stdout receipt."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path
import tempfile


def publish_conversion(output_path: Path, payload: bytes, summary: str) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    temporary_path: Path | None = None
    try:
        with tempfile.NamedTemporaryFile(mode="wb", prefix=f".{output_path.name}.", suffix=".tmp", dir=output_path.parent, delete=False) as output:
            temporary_path = Path(output.name)
            output.write(payload)
            output.flush()
            os.fsync(output.fileno())
        temporary_path.replace(output_path)
    finally:
        if temporary_path is not None:
            temporary_path.unlink(missing_ok=True)

    print(summary)
    print(f"{output_path.name} sha256={hashlib.sha256(payload).hexdigest()}")
