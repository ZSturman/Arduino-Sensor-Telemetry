from __future__ import annotations

import csv
import json
from datetime import datetime
from pathlib import Path

from app.models.telemetry import TelemetryEnvelope


class TelemetryLogger:
    def __init__(self, logs_dir: Path) -> None:
        self._logs_dir = logs_dir
        self._mode = "off"
        self._jsonl_path: Path | None = None
        self._csv_path: Path | None = None
        self._csv_fields: list[str] = []

    def configure(self, mode: str) -> None:
        self._logs_dir.mkdir(parents=True, exist_ok=True)
        self._mode = mode
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        if mode in {"jsonl", "jsonl_csv"}:
            self._jsonl_path = self._logs_dir / f"telemetry-{timestamp}.jsonl"
        else:
            self._jsonl_path = None

        if mode == "jsonl_csv":
            self._csv_path = self._logs_dir / f"telemetry-{timestamp}.csv"
            self._csv_fields = ["timestamp_ms", "sequence", "device_id", "transport"]
        else:
            self._csv_path = None
            self._csv_fields = []

    def log(self, envelope: TelemetryEnvelope) -> None:
        if self._mode == "off":
            return

        payload = envelope.model_dump(mode="json")
        if self._jsonl_path is not None:
            with self._jsonl_path.open("a", encoding="utf-8") as handle:
                handle.write(json.dumps(payload, ensure_ascii=True))
                handle.write("\n")

        if self._csv_path is None or envelope.message_type != "telemetry" or envelope.metrics is None:
            return

        row = {
            "timestamp_ms": envelope.timestamp_ms,
            "sequence": envelope.sequence,
            "device_id": envelope.device_id,
            "transport": envelope.transport,
            **envelope.metrics,
        }
        self._write_csv_row(row)

    def _write_csv_row(self, row: dict[str, object]) -> None:
        assert self._csv_path is not None

        new_fields = [field for field in row.keys() if field not in self._csv_fields]
        if new_fields:
            self._rewrite_csv_with_new_fields(new_fields)

        file_exists = self._csv_path.exists()
        with self._csv_path.open("a", encoding="utf-8", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=self._csv_fields)
            if not file_exists:
                writer.writeheader()
            writer.writerow(row)

    def _rewrite_csv_with_new_fields(self, new_fields: list[str]) -> None:
        assert self._csv_path is not None

        old_rows: list[dict[str, str]] = []
        if self._csv_path.exists():
            with self._csv_path.open("r", encoding="utf-8", newline="") as handle:
                reader = csv.DictReader(handle)
                old_rows = list(reader)

        self._csv_fields.extend(new_fields)

        with self._csv_path.open("w", encoding="utf-8", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=self._csv_fields)
            writer.writeheader()
            for existing in old_rows:
                writer.writerow(existing)

