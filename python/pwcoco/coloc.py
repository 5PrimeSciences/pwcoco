from pathlib import Path

NUMERIC_COLS = {"nsnps", "H0", "H1", "H2", "H3", "H4", "log_abf_all"}


def _coerce_row(raw: dict[str, str]) -> dict:
    row = {}
    for key, val in raw.items():
        if key in NUMERIC_COLS:
            row[key] = int(val) if key == "nsnps" else float(val)
        else:
            row[key] = val
    return row


def parse_coloc(path: str | Path) -> dict:
    path = Path(path)
    rows = []
    with path.open() as fh:
        header = fh.readline().strip().split("\t")
        for line in fh:
            raw = dict(zip(header, line.strip().split("\t")))
            rows.append(_coerce_row(raw))
    return {"path": str(path), "rows": rows}
