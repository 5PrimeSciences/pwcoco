#!/usr/bin/env bash
# Regenerate golden expected outputs. Run manually after intentional algorithm changes.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

if [[ ! -x build/pwcoco ]]; then
  echo "Build pwcoco first: cmake -B build && cmake --build build"
  exit 1
fi

OUT="$ROOT/tests/fixtures/atp1a4/expected/atp1a4"
INPUT="$ROOT/tests/fixtures/atp1a4/input"

echo "WARNING: overwriting golden files in tests/fixtures/atp1a4/expected/"
read -r -p "Continue? [y/N] " ans
[[ "$ans" == "y" || "$ans" == "Y" ]] || exit 1

rm -f "${OUT}.coloc" "${OUT}".*.included

build/pwcoco \
  --bfile "$INPUT/atp1a4_test" \
  --sum_stats1 "$INPUT/atp1a4_test.exp.txt" \
  --sum_stats2 "$INPUT/atp1a4_test.out.txt" \
  --out "$OUT" \
  --maf 0.0001 \
  --init_h4 80 \
  --top_snp 5 \
  --freq_threshold 0.5 \
  --threads 1 \
  --log /tmp/pwcoco_golden_log

echo "Golden files updated. Review git diff before committing."
