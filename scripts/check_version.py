#!/usr/bin/env python3
"""Verify version in _version.py matches CHANGELOG.md and is valid semver."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION_FILE = ROOT / "python" / "pwcoco" / "_version.py"
CHANGELOG = ROOT / "CHANGELOG.md"

SEMVER_RE = re.compile(
    r"^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"
)


def main() -> int:
    version_text = VERSION_FILE.read_text()
    match = re.search(r'__version__ = "(.+)"', version_text)
    if not match:
        print(f"Could not parse __version__ from {VERSION_FILE}", file=sys.stderr)
        return 1

    version = match.group(1)
    if not SEMVER_RE.match(version):
        print(f"Invalid semver: {version!r}", file=sys.stderr)
        return 1

    changelog = CHANGELOG.read_text()
    if not re.search(rf"## \[{re.escape(version)}\]", changelog):
        print(f"CHANGELOG.md missing ## [{version}] section", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
