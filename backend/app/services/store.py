from __future__ import annotations

from collections import defaultdict, deque

from app.models.telemetry import HistoryPoint, RecentTelemetryResponse, TelemetryEnvelope


class TelemetryStore:
    def __init__(self, history_limit: int, event_limit: int, raw_limit: int) -> None:
        self._history_limit = history_limit
        self._recent_events: deque[TelemetryEnvelope] = deque(maxlen=event_limit)
        self._raw_console: deque[str] = deque(maxlen=raw_limit)
        self._numeric_history: dict[str, deque[HistoryPoint]] = defaultdict(lambda: deque(maxlen=history_limit))
        self._latest_metrics: dict[str, object] = {}
        self._sensor_status: dict[str, dict[str, object]] = {}
        self._latest_message: TelemetryEnvelope | None = None

    def ingest(self, envelope: TelemetryEnvelope) -> None:
        if envelope.raw_line:
            self._raw_console.append(envelope.raw_line)
        self._latest_message = envelope

        if envelope.message_type == "telemetry" and envelope.metrics is not None:
            self._latest_metrics.update(envelope.metrics)
            self._sensor_status = envelope.sensor_status or {}
            for key, value in envelope.metrics.items():
                if isinstance(value, bool):
                    numeric_value = 1.0 if value else 0.0
                elif isinstance(value, (int, float)):
                    numeric_value = float(value)
                else:
                    continue
                self._numeric_history[key].append(
                    HistoryPoint(timestamp_ms=envelope.timestamp_ms, value=numeric_value)
                )
            return

        if envelope.message_type == "event":
            self._recent_events.append(envelope)

    def record_malformed_line(self, raw_line: str, error: str) -> None:
        self._raw_console.append(f"{raw_line}    // parse_error: {error}")

    def snapshot(self) -> RecentTelemetryResponse:
        return RecentTelemetryResponse(
            latest_metrics=self._latest_metrics,
            sensor_status=self._sensor_status,
            numeric_history={key: list(points) for key, points in self._numeric_history.items()},
            recent_events=list(self._recent_events),
            raw_console=list(self._raw_console),
            latest_message=self._latest_message,
        )

