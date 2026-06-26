# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-06-26

### Added

- feat: nanobind Python bindings with `pwcoco.run()` returning structured dict results
- feat: pip-installable package via scikit-build-core (`pip install -ve .`)
- feat: Python CLI entry point mirroring C++ flags
- feat: expose `Phenotype`, `Reference`, `initial_coloc`, and `pwcoco_sub` for programmatic use
- feat: golden e2e regression tests for CLI and Python API
- feat: pre-commit hooks (ruff, clang-format, version/changelog check)
- feat: GitHub Actions CI (lint, format, build, golden tests)

### Changed

- refactor: extract C++ orchestration into `pwcoco_core` library (`pwcoco_run.cpp`)
- refactor: slim `options.cpp` to CLI parsing only
- refactor: replace raw pointer ownership with `std::unique_ptr` in run path

### Removed

- remove: embedded Python locus plotter (`python/locusplotter.py`) and `PYTHON_INC` dead code

[1.2.0]: https://github.com/jwr-git/pwcoco/compare/v1.1.1...v1.2.0
