"""Shared helpers for PWCoCo regression tests."""

from __future__ import annotations

from pathlib import Path

import pytest

NUMERIC_COLS = {"nsnps", "H0", "H1", "H2", "H3", "H4", "log_abf_all"}


def load_coloc_rows(path: Path) -> list[dict]:
    rows: list[dict] = []
    with path.open() as fh:
        header = fh.readline().strip().split("\t")
        for line in fh:
            vals = line.strip().split("\t")
            row = {}
            for key, val in zip(header, vals):
                if key in NUMERIC_COLS:
                    row[key] = int(val) if key == "nsnps" else float(val)
                else:
                    row[key] = val
            rows.append(row)
    return rows


def assert_coloc_matches(actual: Path, expected: Path) -> None:
    act = load_coloc_rows(actual)
    exp = load_coloc_rows(expected)
    assert len(act) == len(exp), f"row count: {len(act)} != {len(exp)}"
    for a, e in zip(act, exp):
        for key in ("Dataset1", "Dataset2", "SNP1", "SNP2"):
            assert a[key] == e[key], f"{key}: {a[key]!r} != {e[key]!r}"
        assert a["nsnps"] == e["nsnps"]
        for key in ("H0", "H1", "H2", "H3", "H4", "log_abf_all"):
            assert a[key] == pytest.approx(e[key], rel=1e-6, abs=1e-9)


def assert_dict_rows_match(rows: list[dict], expected: list[dict], *, rel: float = 1e-6) -> None:
    assert len(rows) == len(expected), f"row count: {len(rows)} != {len(expected)}"
    for i, (a, e) in enumerate(zip(rows, expected)):
        for key in ("Dataset1", "Dataset2", "SNP1", "SNP2"):
            assert a[key] == e[key], f"row {i} {key}: {a[key]!r} != {e[key]!r}"
        assert a["nsnps"] == e["nsnps"], f"row {i} nsnps: {a['nsnps']} != {e['nsnps']}"
        for key in ("H0", "H1", "H2", "H3", "H4", "log_abf_all"):
            assert a[key] == pytest.approx(e[key], rel=rel, abs=1e-9), (
                f"row {i} {key}: {a[key]} != {e[key]} (diff={a[key] - e[key]})"
            )


def assert_dict_rows_match_coloc_file(
    rows: list[dict], expected_path: Path, *, rel: float = 1e-6
) -> None:
    assert_dict_rows_match(rows, load_coloc_rows(expected_path), rel=rel)


def assert_included_matches(actual: Path, expected: Path) -> None:
    assert actual.read_text() == expected.read_text()
