from __future__ import annotations

import asyncio
from datetime import datetime, timezone
from typing import Any

from app.core.settings import Settings
from app.models.connection import ConnectRequest, ConnectionState, TransportDescriptor, TransportEndpoint
from app.models.telemetry import TelemetryEnvelope
from app.services.catalog import CatalogService
from app.services.logging import TelemetryLogger
from app.services.ndjson import MalformedEnvelopeError, NDJSONEnvelopeParser
from app.services.store import TelemetryStore
from app.services.websocket_hub import WebSocketHub
from app.transports.base import TransportAdapter
from app.transports.serial_usb import USBSerialTransport
from app.transports.simulator import SimulatorTransport


class TelemetryRuntime:
    def __init__(self, settings: Settings) -> None:
        self.settings = settings
        self.catalog = CatalogService(settings.shared_dir / "sensor_catalog.json")
        self.logger = TelemetryLogger(settings.logs_dir)
        self.parser = NDJSONEnvelopeParser()
        self.store = TelemetryStore(
            history_limit=settings.telemetry_history_limit,
            event_limit=settings.recent_event_limit,
            raw_limit=settings.raw_console_limit,
        )
        self.websocket_hub = WebSocketHub()
        self.started_at = datetime.now(timezone.utc)
        self.state = ConnectionState()
        self._lock = asyncio.Lock()
        self._transport_adapters: dict[str, TransportAdapter] = {
            "usb_serial": USBSerialTransport(),
            "simulator": SimulatorTransport(),
        }
        self._planned_transports = [
            TransportDescriptor(
                id="bluetooth_serial",
                label="Bluetooth Serial",
                description="Planned next-phase transport that will reuse the same telemetry envelopes and frontend flow.",
                supported=False,
                hardware_backed=True,
                next_phase=True,
            )
        ]
        self._active_transport: TransportAdapter | None = None
        self._last_request: ConnectRequest | None = None
        self._reconnect_task: asyncio.Task[None] | None = None

    async def startup(self) -> None:
        self.settings.logs_dir.mkdir(parents=True, exist_ok=True)

    async def shutdown(self) -> None:
        await self.disconnect()

    async def list_transports(self) -> list[TransportDescriptor]:
        return [adapter.descriptor for adapter in self._transport_adapters.values()] + self._planned_transports

    async def list_endpoints(self, transport_id: str) -> list[TransportEndpoint]:
        adapter = self._transport_adapters.get(transport_id)
        if adapter is None:
            return []
        return await adapter.list_endpoints()

    def health_payload(self) -> dict[str, Any]:
        return {
            "status": "ok",
            "backend_started_at": self.started_at,
            "connection": self.state.model_dump(mode="json"),
        }

    async def connect(self, request: ConnectRequest) -> ConnectionState:
        async with self._lock:
            await self._cancel_reconnect_task()
            adapter = self._transport_adapters.get(request.transport)
            if adapter is None:
                raise ValueError(f"Unsupported or unavailable transport '{request.transport}'.")

            await self._disconnect_active_transport()
            self.logger.configure(request.logging_mode)
            self.state = ConnectionState(
                connection_state="connecting",
                transport=request.transport,
                endpoint=request.endpoint,
                baud_rate=request.baud_rate,
                auto_reconnect=request.auto_reconnect,
                malformed_messages=self.state.malformed_messages,
                device_id=self.state.device_id,
                last_sequence=self.state.last_sequence,
                last_message_at=self.state.last_message_at,
            )
            await self._broadcast_connection_state()

            await adapter.connect(request, self._handle_line, self._handle_transport_disconnect)
            self._active_transport = adapter
            self._last_request = request
            self.state.connection_state = "connected"
            self.state.last_error = None
            await self._broadcast_connection_state()
            return self.state

    async def disconnect(self) -> ConnectionState:
        async with self._lock:
            await self._cancel_reconnect_task()
            await self._disconnect_active_transport()
            self.state.connection_state = "disconnected"
            self.state.transport = None
            self.state.endpoint = None
            self.state.baud_rate = None
            self.state.auto_reconnect = False
            await self._broadcast_connection_state()
            return self.state

    async def _disconnect_active_transport(self) -> None:
        if self._active_transport is not None:
            await self._active_transport.disconnect()
            self._active_transport = None

    async def _handle_line(self, raw_line: str) -> None:
        try:
            envelope = self.parser.parse_line(raw_line)
        except MalformedEnvelopeError as exc:
            self.state.malformed_messages += 1
            self.state.last_error = str(exc)
            self.store.record_malformed_line(raw_line, str(exc))
            await self.websocket_hub.broadcast(
                {
                    "type": "parse_error",
                    "payload": {
                        "raw_line": raw_line,
                        "error": str(exc),
                        "malformed_messages": self.state.malformed_messages,
                    },
                }
            )
            await self._broadcast_connection_state()
            return

        envelope.received_at = datetime.now(timezone.utc)
        envelope.raw_line = raw_line
        self.logger.log(envelope)
        self.store.ingest(envelope)

        self.state.device_id = envelope.device_id
        self.state.last_sequence = envelope.sequence
        self.state.last_message_at = envelope.received_at
        self.state.last_error = None

        await self.websocket_hub.broadcast(
            {"type": "envelope", "payload": envelope.model_dump(mode="json")}
        )
        await self._broadcast_connection_state()

    async def _handle_transport_disconnect(self, reason: str | None) -> None:
        self.state.connection_state = "disconnected"
        self.state.last_error = reason
        await self._broadcast_connection_state()

        if (
            self._last_request is not None
            and self._last_request.auto_reconnect
            and self._last_request.transport == "usb_serial"
        ):
            self.state.connection_state = "reconnecting"
            await self._broadcast_connection_state()
            self._reconnect_task = asyncio.create_task(self._attempt_reconnect())

    async def _attempt_reconnect(self) -> None:
        while self._last_request is not None and self._last_request.auto_reconnect:
            await asyncio.sleep(self.settings.reconnect_interval_seconds)
            try:
                await self.connect(self._last_request)
                return
            except Exception as exc:
                self.state.connection_state = "reconnecting"
                self.state.last_error = str(exc)
                await self._broadcast_connection_state()

    async def _cancel_reconnect_task(self) -> None:
        if self._reconnect_task is not None:
            self._reconnect_task.cancel()
            try:
                await self._reconnect_task
            except asyncio.CancelledError:
                pass
            self._reconnect_task = None

    async def _broadcast_connection_state(self) -> None:
        await self.websocket_hub.broadcast(
            {"type": "connection_state", "payload": self.state.model_dump(mode="json")}
        )

