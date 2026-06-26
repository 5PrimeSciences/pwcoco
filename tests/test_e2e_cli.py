"""End-to-end CLI regression tests against golden outputs."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

from tests.helpers import assert_coloc_matches, assert_included_matches

FIXTURE = Path(__file__).resolve().parent / "fixtures" / "atp1a4"
INPUT = FIXTURE / "input"
EXPECTED = FIXTURE / "expected"


def test_cli_matches_golden(pwcoco_bin: Path, tmp_path: Path) -> None:
    cfg = json.loads((EXPECTED / "run_config.json").read_text())
    out_prefix = tmp_path / "run"

    cmd = [
        str(pwcoco_bin),
        "--bfile",
        str(INPUT / "atp1a4_test"),
        "--sum_stats1",
        str(INPUT / "atp1a4_test.exp.txt"),
        "--sum_stats2",
        str(INPUT / "atp1a4_test.out.txt"),
        "--out",
        str(out_prefix),
        "--log",
        str(tmp_path / "pwcoco_log"),
        *cfg["extra_args"],
    ]
    subprocess.run(cmd, check=True, capture_output=True, text=True)

    assert_coloc_matches(out_prefix.with_suffix(".coloc"), EXPECTED / cfg["coloc_file"])
    for name in cfg["included_files"]:
        suffix = name.split(".", 1)[1]
        actual = out_prefix.parent / f"{out_prefix.name}.{suffix}"
        assert_included_matches(actual, EXPECTED / name)
