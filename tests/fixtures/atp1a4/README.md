# ATP1A4 golden fixture

Minimal synthetic Plink panel + sum stats from `data/atp1a4_test.{exp,out}.txt`.

## Regenerate golden outputs

```bash
cmake -B build && cmake --build build
./scripts/generate_golden.sh
```

Review diffs carefully before committing. Golden files must only change when numerics intentionally change.

## Pinned CLI args

See `expected/run_config.json` (`--threads 1` for reproducibility).
