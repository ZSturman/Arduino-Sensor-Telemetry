from __future__ import annotations

from datetime import datetime
from typing import Literal

from pydantic import BaseModel, ConfigDict, Field, model_validator

ScalarValue = str | int | float | bool | None


class EventDetails(BaseModel):
    source: str
    name: str
    fields: dict[str, ScalarValue] = Field(default_factory=dict)


class TelemetryEnvelope(BaseModel):
    model_config = ConfigDict(extra="ignore")

    schema_version: str
    message_type: Literal["telemetry", "event"]
    timestamp_ms: int = Field(ge=0)
    sequence: int = Field(ge=0)
    device_id: str
    transport: str
    metrics: dict[str, ScalarValue] | None = None
    sensor_status: dict[str, dict[str, ScalarValue]] | None = None
    event: EventDetails | None = None
    received_at: datetime | None = None
    raw_line: str | None = None

    @model_validator(mode="after")
    def validate_payload_shape(self) -> "TelemetryEnvelope":
        if self.message_type == "telemetry":
            if self.metrics is None or self.sensor_status is None:
                raise ValueError("Telemetry messages require metrics and sensor_status.")
        if self.message_type == "event" and self.event is None:
            raise ValueError("Event messages require an event payload.")
        return self


class HistoryPoint(BaseModel):
    timestamp_ms: int
    value: float


class RecentTelemetryResponse(BaseModel):
    latest_metrics: dict[str, ScalarValue] = Field(default_factory=dict)
    sensor_status: dict[str, dict[str, ScalarValue]] = Field(default_factory=dict)
    numeric_history: dict[str, list[HistoryPoint]] = Field(default_factory=dict)
    recent_events: list[TelemetryEnvelope] = Field(default_factory=list)
    raw_console: list[str] = Field(default_factory=list)
    latest_message: TelemetryEnvelope | None = None

