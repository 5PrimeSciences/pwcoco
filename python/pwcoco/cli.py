from __future__ import annotations

import argparse
import sys

from pwcoco._pwcoco import PwCoCoConfig, run_core


def _add_common_args(p: argparse.ArgumentParser) -> None:
    p.add_argument("--bfile", required=True)
    p.add_argument("--sum_stats1", required=True)
    p.add_argument("--sum_stats2", required=True)
    p.add_argument("--out", default="pwcoco_out")
    p.add_argument("--log", default="pwcoco_log")
    p.add_argument("--p_cutoff", type=float)
    p.add_argument("--p_cutoff1", type=float)
    p.add_argument("--p_cutoff2", type=float)
    p.add_argument("--chr", type=int)
    p.add_argument("--top_snp", type=float)
    p.add_argument("--ld_window", type=int)
    p.add_argument("--collinear", type=float)
    p.add_argument("--maf", type=float)
    p.add_argument("--freq_threshold", type=float)
    p.add_argument("--init_h4", type=float)
    p.add_argument("--coloc_pp", nargs=3, type=float)
    p.add_argument("--n1", type=float)
    p.add_argument("--n2", type=float)
    p.add_argument("--n1_case", type=float)
    p.add_argument("--n2_case", type=float)
    p.add_argument("--threads", type=int)
    p.add_argument("--pve1", type=float)
    p.add_argument("--pve2", type=float)
    p.add_argument("--pve_file1")
    p.add_argument("--pve_file2")
    p.add_argument("--out_cond", action="store_true")
    p.add_argument("--cond_ssize", action="store_true")
    p.add_argument("--verbose", action="store_true")
    p.add_argument("--pairwise", action="store_true")


def _config_from_args(args: argparse.Namespace) -> PwCoCoConfig:
    cfg = PwCoCoConfig()
    cfg.bfile = args.bfile
    if cfg.bfile.endswith(".bfile"):
        cfg.bfile = cfg.bfile[:-6]
    cfg.bim_file = cfg.bfile + ".bim"
    cfg.fam_file = cfg.bfile + ".fam"
    cfg.bed_file = cfg.bfile + ".bed"
    cfg.sum_stats1 = args.sum_stats1
    cfg.sum_stats2 = args.sum_stats2
    cfg.out = args.out
    cfg.log = args.log
    if args.p_cutoff is not None:
        cfg.p_cutoff1 = cfg.p_cutoff2 = args.p_cutoff
    if args.p_cutoff1 is not None:
        cfg.p_cutoff1 = args.p_cutoff1
    if args.p_cutoff2 is not None:
        cfg.p_cutoff2 = args.p_cutoff2
    if args.chr is not None:
        cfg.chr = args.chr
    if args.top_snp is not None:
        cfg.top_snp = args.top_snp
    if args.ld_window is not None:
        cfg.ld_window = args.ld_window * 1000
    if args.collinear is not None:
        cfg.collinear = args.collinear
    if args.maf is not None:
        cfg.maf = args.maf
    if args.freq_threshold is not None:
        cfg.freq_threshold = args.freq_threshold
    if args.init_h4 is not None:
        cfg.init_h4 = args.init_h4
    if args.coloc_pp is not None:
        cfg.p1, cfg.p2, cfg.p3 = args.coloc_pp
    if args.n1 is not None:
        cfg.n1 = args.n1
    if args.n2 is not None:
        cfg.n2 = args.n2
    if args.n1_case is not None:
        cfg.n1_case = args.n1_case
    if args.n2_case is not None:
        cfg.n2_case = args.n2_case
    if args.threads is not None:
        cfg.threads = args.threads
    if args.pve1 is not None:
        cfg.pve1 = args.pve1
    if args.pve2 is not None:
        cfg.pve2 = args.pve2
    if args.pve_file1 is not None:
        cfg.pve_file1 = args.pve_file1
    if args.pve_file2 is not None:
        cfg.pve_file2 = args.pve_file2
    cfg.out_cond = args.out_cond
    cfg.cond_ssize = args.cond_ssize
    cfg.verbose = args.verbose
    cfg.pairwise = args.pairwise
    if cfg.verbose:
        cfg.out_cond = True
    return cfg


def main() -> None:
    parser = argparse.ArgumentParser(prog="pwcoco")
    _add_common_args(parser)
    args = parser.parse_args()
    cfg = _config_from_args(args)
    sys.exit(run_core(cfg))


if __name__ == "__main__":
    main()
