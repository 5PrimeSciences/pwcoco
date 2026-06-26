from pwcoco._pwcoco import (
    Phenotype,
    PwCoCoConfig,
    Reference,
    initial_coloc,
    pwcoco_sub,
    run_core as _run_core,
)
from pwcoco._version import __version__
from pwcoco.coloc import parse_coloc


def run(bfile, sum_stats1, sum_stats2, *, out="pwcoco_out", **kwargs):
    cfg = PwCoCoConfig()
    cfg.bfile = bfile
    cfg.sum_stats1 = sum_stats1
    cfg.sum_stats2 = sum_stats2
    cfg.out = out
    if bfile.endswith(".bfile"):
        bfile = bfile[:-6]
    cfg.bfile = bfile
    cfg.bim_file = bfile + ".bim"
    cfg.fam_file = bfile + ".fam"
    cfg.bed_file = bfile + ".bed"
    for key, value in kwargs.items():
        if key == "ld_window" and isinstance(value, (int, float)):
            setattr(cfg, key, int(value) * 1000)
        else:
            setattr(cfg, key, value)
    rc = _run_core(cfg)
    if rc != 0:
        raise RuntimeError(f"PWCoCo failed with exit code {rc}")
    return parse_coloc(f"{out}.coloc")


__all__ = [
    "Phenotype",
    "PwCoCoConfig",
    "Reference",
    "initial_coloc",
    "pwcoco_sub",
    "run",
    "parse_coloc",
    "__version__",
]
