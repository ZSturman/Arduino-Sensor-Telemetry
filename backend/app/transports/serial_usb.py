from __future__ import annotations

import asyncio

import serial
from serial.tools import list_ports

from app.models.connection import ConnectRequest, TransportDescriptor, TransportEndpoint
from app.transports.base import DisconnectHandler, LineHandler, TransportAdapter


class USBSerialTransport(TransportAdapter):
    descriptor = TransportDescriptor(
        id="usb_serial",
        label="USB Serial",
        description="Arduino Mega telemetry over the USB serial port.",
        supported=True,
        hardware_backed=True,
    )

    def __init__(self) -> None:
        self._serial: serial.Serial | None = None
        self._reader_task: asyncio.Task[None] | None = None
        self._stop_requested = False
        self._disconnect_handler: DisconnectHandler | None = None

    async def list_endpoints(self) -> list[TransportEndpoint]:
        endpoints: list[TransportEndpoint] = []
        for port in list_ports.comports():
            endpoints.append(
                TransportEndpoint(
                    id=port.device,
                    label=port.device,
                    description=port.description or "USB serial device",
                    metadata={
                        "hwid": port.hwid,
                        "manufacturer": port.manufacturer,
                    },
                )
            )
        return endpoints

    async def connect(
        self,
        request: ConnectRequest,
        line_handler: LineHandler,
        disconnect_handler: DisconnectHandler,
    ) -> None:
        await self.disconnect()
        if not request.endpoint:
            raise ValueError("USB serial transport requires a port path in endpoint.")

        self._stop_requested = False
        self._disconnect_handler = disconnect_handler
        self._serial = await asyncio.to_thread(
            serial.Serial,
            request.endpoint,
            request.baud_rate,
            timeout=0.25,
        )
        self._reader_task = asyncio.create_task(self._reader_loop(line_handler))

    async def _reader_loop(self, line_handler: LineHandler) -> None:
        assert self._serial is not None

        buffer = bytearray()
        try:
            while not self._stop_requested:
                chunk = await asyncio.to_thread(self._serial.read, self._serial.in_waiting or 1)
                if not chunk:
                    continue
                buffer.extend(chunk)

                while b"\n" in buffer:
                    raw, _, remainder = buffer.partition(b"\n")
                    buffer = bytearray(remainder)
                    decoded = raw.decode("utf-8", errors="replace").strip()
                    if decoded:
                        await line_handler(decoded)
        except serial.SerialException as exc:
            if self._disconnect_handler is not None and not self._stop_requested:
                await self._disconnect_handler(str(exc))
        finally:
            if self._serial is not None and self._serial.is_open:
                await asyncio.to_thread(self._serial.close)

    async def disconnect(self) -> None:
        self._stop_requested = True
        if self._reader_task is not None:
            self._reader_task.cancel()
            try:
                await self._reader_task
            except asyncio.CancelledError:
                pass
            self._reader_task = None

        if self._serial is not None:
            if self._serial.is_open:
                await asyncio.to_thread(self._serial.close)
            self._serial = None

