from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Settings:
    project_root: Path
    shared_dir: Path
    logs_dir: Path
    telemetry_history_limit: int = 120
    recent_event_limit: int = 50
    raw_console_limit: int = 100
    reconnect_interval_seconds: float = 3.0


def get_settings() -> Settings:
    project_root = Path(__file__).resolve().parents[3]
    return Settings(
        project_root=project_root,
        shared_dir=project_root / "shared",
        logs_dir=project_root / "backend" / "logs",
    )

