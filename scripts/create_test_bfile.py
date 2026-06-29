#!/usr/bin/env python3
"""Create a minimal synthetic Plink bfile from PWCoCo sum-stats fixtures."""

from __future__ import annotations

import argparse
from pathlib import Path


def read_sumstats(path: Path) -> dict[str, tuple[str, str, float]]:
    """Return SNP -> (A1, A2, EAF) from tab-delimited sum stats."""
    snps: dict[str, tuple[str, str, float]] = {}
    with path.open() as fh:
        header = fh.readline()
        if "SNP" not in header:
            raise ValueError(f"Unexpected header in {path}")
        for line in fh:
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 7:
                continue
            snp, a1, a2 = parts[0], parts[1].upper(), parts[2].upper()
            try:
                eaf = float(parts[3])
            except ValueError:
                continue
            if snp and snp not in {".", "NA"}:
                snps[snp] = (a1, a2, eaf)
    return snps


def genotype_byte(genotypes: list[int]) -> bytes:
    """Pack 2-bit PLINK genotypes (4 per byte). 0=hom ref, 2=hom alt, 1=het, 3=missing."""
    out = bytearray()
    for i in range(0, len(genotypes), 4):
        chunk = genotypes[i : i + 4]
        while len(chunk) < 4:
            chunk.append(1)  # het pad
        val = 0
        for j, g in enumerate(chunk):
            val |= (g & 3) << (2 * j)
        out.append(val)
    return bytes(out)


def eaf_to_genotypes(eaf: float, n_ind: int, seed_offset: int) -> list[int]:
    """Deterministic pseudo-random genotypes matching target allele frequency."""
    alt_count = int(round(2 * eaf * n_ind))
    # Build alt allele dosage list then shuffle deterministically
    dosages = [2] * (alt_count // 2)
    if alt_count % 2:
        dosages.append(1)
    dosages.extend([0] * (n_ind - len(dosages)))
    # Simple deterministic reorder (LCG-style) for reproducibility
    order = list(range(n_ind))
    state = (seed_offset * 1103515245 + 12345) & 0x7FFFFFFF
    for i in range(n_ind - 1, 0, -1):
        state = (state * 1103515245 + 12345) & 0x7FFFFFFF
        j = state % (i + 1)
        order[i], order[j] = order[j], order[i]
    shuffled = [0] * n_ind
    for idx, orig in enumerate(order):
        shuffled[orig] = dosages[idx]
    return shuffled


def write_bfile(prefix: Path, snps: list[tuple[str, str, str, float]], n_ind: int) -> None:
    prefix.parent.mkdir(parents=True, exist_ok=True)

    with (prefix.with_suffix(".bim")).open("w") as bim:
        for i, (snp, a1, a2, _eaf) in enumerate(snps):
            bim.write(f"1\t{snp}\t0\t{1_000_000 + i * 1000}\t{a1}\t{a2}\n")

    with (prefix.with_suffix(".fam")).open("w") as fam:
        for i in range(n_ind):
            fam.write(f"FAM\tIND{i}\t0\t0\t0\t-9\n")

    bed_path = prefix.with_suffix(".bed")
    with bed_path.open("wb") as bed:
        bed.write(bytes([0x6C, 0x1B, 0x01]))  # PLINK bed magic
        for i, (_snp, _a1, _a2, eaf) in enumerate(snps):
            genotypes = eaf_to_genotypes(eaf, n_ind, i)
            bed.write(genotype_byte(genotypes))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--exp", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--prefix", type=Path, required=True)
    parser.add_argument("--n-ind", type=int, default=100)
    args = parser.parse_args()

    exp = read_sumstats(args.exp)
    out = read_sumstats(args.out)
    common = sorted(set(exp) & set(out))
    if not common:
        raise SystemExit("No overlapping SNPs between exposure and outcome files")

    snp_records: list[tuple[str, str, str, float]] = []
    for snp in common:
        a1_e, a2_e, eaf_e = exp[snp]
        a1_o, a2_o, eaf_o = out[snp]
        # Use exposure alleles; average EAF if alleles match orientation
        if (a1_e, a2_e) == (a1_o, a2_o):
            eaf = (eaf_e + eaf_o) / 2
        elif (a1_e, a2_e) == (a2_o, a1_o):
            eaf = 1 - (eaf_e + eaf_o) / 2
        else:
            eaf = eaf_e
        snp_records.append((snp, a1_e, a2_e, eaf))

    write_bfile(args.prefix, snp_records, args.n_ind)
    print(
        f"Wrote {len(snp_records)} SNPs, {args.n_ind} individuals -> {args.prefix}.{{bed,bim,fam}}"
    )


if __name__ == "__main__":
    main()
