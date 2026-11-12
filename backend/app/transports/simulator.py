from __future__ import annotations

import asyncio
import json
import math
import random
import time

from app.models.connection import ConnectRequest, TransportDescriptor, TransportEndpoint
from app.transports.base import DisconnectHandler, LineHandler, TransportAdapter


class SimulatorTransport(TransportAdapter):
    descriptor = TransportDescriptor(
        id="simulator",
        label="Simulator",
        description="Synthetic telemetry stream for frontend and backend development without hardware.",
        supported=True,
        hardware_backed=False,
    )

    def __init__(self) -> None:
        self._task: asyncio.Task[None] | None = None
        self._stop_requested = False

    async def list_endpoints(self) -> list[TransportEndpoint]:
        return [
            TransportEndpoint(
                id="simulator://default",
                label="Default Lab Rig",
                description="Balanced environmental and input activity.",
            ),
            TransportEndpoint(
                id="simulator://busy-room",
                label="Busy Room",
                description="Higher sound and motion activity.",
            ),
            TransportEndpoint(
                id="simulator://weather-spike",
                label="Weather Spike",
                description="More frequent distance and rain variation.",
            ),
        ]

    async def connect(
        self,
        request: ConnectRequest,
        line_handler: LineHandler,
        disconnect_handler: DisconnectHandler,
    ) -> None:
        del disconnect_handler
        await self.disconnect()
        self._stop_requested = False
        profile = request.endpoint or f"simulator://{request.simulator_profile}"
        self._task = asyncio.create_task(self._run(profile, line_handler))

    async def _run(self, profile: str, line_handler: LineHandler) -> None:
        start = time.perf_counter()
        sequence = 0
        event_counter = 0
        keypad_cycle = iter(["1", "2", "3", "A", "B", "C", "*", "#"])
        while not self._stop_requested:
            elapsed = int((time.perf_counter() - start) * 1000)
            phase = elapsed / 1000.0

            motion = 1 if math.sin(phase / 5.0) > 0.65 else 0
            sound_bias = 50 if "busy-room" in profile else 0
            rain_bias = 80 if "weather-spike" in profile else 0

            telemetry_payload = {
                "schema_version": "1.0.0",
                "message_type": "telemetry",
                "timestamp_ms": elapsed,
                "sequence": sequence,
                "device_id": "simulator-mega2560",
                "transport": "simulator",
                "metrics": {
                    "temperature_c": round(23.0 + math.sin(phase / 15.0) * 2.5, 2),
                    "humidity_pct": round(48.0 + math.cos(phase / 10.0) * 6.0, 2),
                    "motion": motion,
                    "distance_cm": round(35.0 + math.sin(phase / 2.0) * 8.0, 2),
                    "sound_raw": int(420 + sound_bias + math.sin(phase * 3.0) * 70 + random.randint(-10, 10)),
                    "rain_raw": int(220 + rain_bias + math.cos(phase / 4.0) * 35 + random.randint(-5, 5)),
                    "rain_detected": 1 if math.sin(phase / 7.0) > 0.75 else 0,
                    "joy_x": int(512 + math.sin(phase) * 240),
                    "joy_y": int(512 + math.cos(phase / 1.4) * 220),
                    "joy_button": 1 if event_counter % 11 == 0 else 0,
                    "touch": 1 if event_counter % 17 == 0 else 0,
                    "encoder_position": event_counter * 2,
                    "encoder_delta": 2,
                    "encoder_button": 1 if event_counter % 13 == 0 else 0,
                    "button_pressed": 1 if event_counter % 9 == 0 else 0,
                    "rfid_last_uid": "A1B2C3D4",
                    "keypad_last_key": "5",
                    "ir_last_code_hex": "0xFFA25D",
                },
                "sensor_status": {
                    "dht11": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "pir_motion": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "hc_sr04": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "sound_sensor": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "rain_sensor": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "joystick": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "touch_sensor": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "rotary_encoder": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "pushbutton": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "ir_receiver": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "rc522_rfid": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                    "keypad_4x4": {"connected": True, "healthy": True, "last_update_ms": elapsed},
                },
            }
            await line_handler(json.dumps(telemetry_payload))
            sequence += 1
            event_counter += 1

            if event_counter % 5 == 0:
                event_payload = {
                    "schema_version": "1.0.0",
                    "message_type": "event",
                    "timestamp_ms": elapsed,
                    "sequence": sequence,
                    "device_id": "simulator-mega2560",
                    "transport": "simulator",
                    "event": {
                        "source": "keypad_4x4",
                        "name": "key_pressed",
                        "fields": {"keypad_last_key": next(keypad_cycle, "1")},
                    },
                }
                await line_handler(json.dumps(event_payload))
                sequence += 1

            if event_counter % 9 == 0:
                event_payload = {
                    "schema_version": "1.0.0",
                    "message_type": "event",
                    "timestamp_ms": elapsed,
                    "sequence": sequence,
                    "device_id": "simulator-mega2560",
                    "transport": "simulator",
                    "event": {
                        "source": "rc522_rfid",
                        "name": "tag_scanned",
                        "fields": {"rfid_last_uid": random.choice(["A1B2C3D4", "BEEFAB12", "1122AAFF"])},
                    },
                }
                await line_handler(json.dumps(event_payload))
                sequence += 1

            await asyncio.sleep(0.5)

    async def disconnect(self) -> None:
        self._stop_requested = True
        if self._task is not None:
            self._task.cancel()
            try:
                await self._task
            except asyncio.CancelledError:
                pass
            self._task = None

