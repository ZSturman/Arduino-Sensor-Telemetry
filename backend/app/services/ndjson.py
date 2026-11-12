from __future__ import annotations

import json

from pydantic import ValidationError

from app.models.telemetry import TelemetryEnvelope


class MalformedEnvelopeError(ValueError):
    """Raised when a serial line is not valid telemetry NDJSON."""


class NDJSONEnvelopeParser:
    def parse_line(self, raw_line: str) -> TelemetryEnvelope:
        line = raw_line.strip()
        if not line:
            raise MalformedEnvelopeError("Empty line.")

        try:
            payload = json.loads(line)
        except json.JSONDecodeError as exc:
            raise MalformedEnvelopeError(f"JSON decode failed: {exc.msg}") from exc

        try:
            envelope = TelemetryEnvelope.model_validate(payload)
        except ValidationError as exc:
            raise MalformedEnvelopeError(f"Envelope validation failed: {exc}") from exc

        return envelope

