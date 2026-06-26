from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[1]
FIXTURE = REPO_ROOT / "tests" / "fixtures" / "atp1a4"
PWCOCO_BIN = REPO_ROOT / "build" / "pwcoco"


@pytest.fixture(scope="session")
def repo_root() -> Path:
    return REPO_ROOT


@pytest.fixture(scope="session")
def fixture_dir() -> Path:
    return FIXTURE


@pytest.fixture(scope="session")
def pwcoco_bin() -> Path:
    if not PWCOCO_BIN.exists():
        pytest.skip(f"pwcoco binary not built: {PWCOCO_BIN}")
    return PWCOCO_BIN
