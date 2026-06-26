"""End-to-end Python API regression tests against golden outputs."""

from __future__ import annotations

import json
from pathlib import Path

import pwcoco
import pytest

from tests.helpers import assert_dict_rows_match_coloc_file

FIXTURE = Path(__file__).resolve().parent / "fixtures" / "atp1a4"
INPUT = FIXTURE / "input"
EXPECTED = FIXTURE / "expected"


@pytest.fixture(scope="module")
def _require_pwcoco_extension() -> None:
    try:
        import pwcoco._pwcoco  # noqa: F401
    except ImportError:
        pytest.skip("pwcoco extension not built; run pip install -ve .")


def test_python_api_matches_golden(_require_pwcoco_extension, tmp_path: Path) -> None:
    cfg = json.loads((EXPECTED / "run_config.json").read_text())
    out = tmp_path / "run"
    result = pwcoco.run(
        bfile=str(INPUT / "atp1a4_test"),
        sum_stats1=str(INPUT / "atp1a4_test.exp.txt"),
        sum_stats2=str(INPUT / "atp1a4_test.out.txt"),
        out=str(out),
        log=str(tmp_path / "pwcoco_log"),
        **cfg["kwargs"],
    )
    assert_dict_rows_match_coloc_file(result["rows"], EXPECTED / cfg["coloc_file"])
