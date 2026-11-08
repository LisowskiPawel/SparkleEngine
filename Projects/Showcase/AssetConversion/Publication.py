"""Publish one complete conversion artifact and its stdout receipt."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path


def publish_conversion(output_path: Path, payload: bytes, summary: str) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    temporary_path = output_path.with_name(f".{output_path.name}.tmp")
    try:
        with temporary_path.open("wb") as output:
            output.write(payload)
            output.flush()
            os.fsync(output.fileno())
        temporary_path.replace(output_path)
    finally:
        temporary_path.unlink(missing_ok=True)

    print(summary)
    print(f"{output_path.name} sha256={hashlib.sha256(payload).hexdigest()}")
