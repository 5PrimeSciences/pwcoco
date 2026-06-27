"""End-to-end Python API regression tests against the packaged C++ CLI."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

import pwcoco
import pwcoco._pwcoco
import pytest

from tests.helpers import assert_dict_rows_match, load_coloc_rows

FIXTURE = Path(__file__).resolve().parent / "fixtures" / "atp1a4"
INPUT = FIXTURE / "input"
EXPECTED = FIXTURE / "expected"


def _bundled_pwcoco_binary() -> Path:
    bin_path = Path(pwcoco._pwcoco.__file__).resolve().parent / "bin" / "pwcoco"
    if not bin_path.exists():
        pytest.skip(f"bundled pwcoco binary not found: {bin_path}")
    return bin_path


@pytest.fixture(scope="module")
def _require_pwcoco_extension() -> None:
    try:
        import pwcoco._pwcoco  # noqa: F401
    except ImportError:
        pytest.skip("pwcoco extension not built; run pip install -ve .")


def test_python_api_matches_packaged_cli(_require_pwcoco_extension, tmp_path: Path) -> None:
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

    cli_out = tmp_path / "cli_run"
    cmd = [
        str(_bundled_pwcoco_binary()),
        "--bfile",
        str(INPUT / "atp1a4_test"),
        "--sum_stats1",
        str(INPUT / "atp1a4_test.exp.txt"),
        "--sum_stats2",
        str(INPUT / "atp1a4_test.out.txt"),
        "--out",
        str(cli_out),
        "--log",
        str(tmp_path / "cli_log"),
        *cfg["extra_args"],
    ]
    subprocess.run(cmd, check=True, capture_output=True, text=True)

    cli_rows = load_coloc_rows(cli_out.with_suffix(".coloc"))
    # Embedded extension vs subprocess CLI can differ slightly on Linux FP/OpenMP env.
    assert_dict_rows_match(result["rows"], cli_rows, rel=1e-4)
