from pwcoco._pwcoco import (
    Phenotype,
    PwCoCoConfig,
    Reference,
    config_from_cli_args,
    initial_coloc,
    pwcoco_sub,
)
from pwcoco._pwcoco import (
    run_core as _run_core,
)
from pwcoco._version import __version__
from pwcoco.coloc import parse_coloc

_FLAG_KWARGS = {
    "p_cutoff": "--p_cutoff",
    "p_cutoff1": "--p_cutoff1",
    "p_cutoff2": "--p_cutoff2",
    "chr": "--chr",
    "top_snp": "--top_snp",
    "collinear": "--collinear",
    "maf": "--maf",
    "freq_threshold": "--freq_threshold",
    "init_h4": "--init_h4",
    "n1": "--n1",
    "n2": "--n2",
    "n1_case": "--n1_case",
    "n2_case": "--n2_case",
    "threads": "--threads",
    "pve1": "--pve1",
    "pve2": "--pve2",
    "pve_file1": "--pve_file1",
    "pve_file2": "--pve_file2",
}
_BOOL_KWARGS = ("out_cond", "cond_ssize", "verbose", "pairwise")


def _cli_args_from_run(
    bfile: str,
    sum_stats1: str,
    sum_stats2: str,
    *,
    out: str = "pwcoco_out",
    log: str = "pwcoco_log",
    **kwargs,
) -> list[str]:
    args = [
        "--bfile",
        bfile,
        "--sum_stats1",
        sum_stats1,
        "--sum_stats2",
        sum_stats2,
        "--out",
        out,
        "--log",
        log,
    ]
    for key, value in kwargs.items():
        if key == "ld_window":
            args.extend(["--ld_window", str(int(value))])
        elif key == "coloc_pp":
            args.extend(["--coloc_pp", *(str(v) for v in value)])
        elif key in _BOOL_KWARGS:
            if value:
                args.append(f"--{key}")
        elif key in _FLAG_KWARGS:
            args.extend([_FLAG_KWARGS[key], str(value)])
        else:
            raise TypeError(f"unexpected keyword argument: {key!r}")
    return args


def run(bfile, sum_stats1, sum_stats2, *, out="pwcoco_out", log="pwcoco_log", **kwargs):
    cfg = config_from_cli_args(
        _cli_args_from_run(bfile, sum_stats1, sum_stats2, out=out, log=log, **kwargs)
    )
    rc = _run_core(cfg)
    if rc != 0:
        raise RuntimeError(f"PWCoCo failed with exit code {rc}")
    return parse_coloc(f"{out}.coloc")


__all__ = [
    "Phenotype",
    "PwCoCoConfig",
    "Reference",
    "config_from_cli_args",
    "initial_coloc",
    "pwcoco_sub",
    "run",
    "parse_coloc",
    "__version__",
]
