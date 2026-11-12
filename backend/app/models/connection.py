from __future__ import annotations

from datetime import datetime
from typing import Any, Literal

from pydantic import BaseModel, Field


class TransportEndpoint(BaseModel):
    id: str
    label: str
    description: str
    available: bool = True
    metadata: dict[str, Any] = Field(default_factory=dict)


class TransportDescriptor(BaseModel):
    id: str
    label: str
    description: str
    supported: bool
    hardware_backed: bool
    next_phase: bool = False


class ConnectRequest(BaseModel):
    transport: str
    endpoint: str | None = None
    baud_rate: int = 115200
    auto_reconnect: bool = True
    logging_mode: Literal["off", "jsonl", "jsonl_csv"] = "jsonl"
    simulator_profile: str = "default"


class ConnectionState(BaseModel):
    backend_connected: bool = True
    connection_state: Literal["disconnected", "connecting", "connected", "reconnecting", "error"] = "disconnected"
    transport: str | None = None
    endpoint: str | None = None
    baud_rate: int | None = None
    auto_reconnect: bool = False
    malformed_messages: int = 0
    last_error: str | None = None
    device_id: str | None = None
    last_sequence: int | None = None
    last_message_at: datetime | None = None

