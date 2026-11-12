# System Architecture

## Responsibilities

### Arduino Mega firmware

- Samples enabled sensors and input modules on predictable intervals
- Normalizes readings into stable field names
- Emits newline-delimited JSON over USB serial
- Produces two message types:
  - `telemetry` snapshots for the latest sensor state
  - `event` records for discrete actions such as key presses, RFID scans, IR frames, and button edges
- Centralizes pin assignments and sensor toggles in `AppConfig.h`

### Python backend

- Discovers available transport endpoints
- Connects to the selected transport
- Buffers and parses NDJSON safely
- Validates envelopes against the backend data model
- Tracks connection state, malformed message counts, and reconnect behavior
- Broadcasts live updates to the frontend over WebSocket
- Exposes catalog, connection, health, and recent telemetry endpoints
- Supports simulator mode for development without hardware

### React frontend

- Shows backend and transport connection state
- Lets the operator select the transport, endpoint, baud rate, and logging mode
- Renders known sensor cards from the shared catalog
- Auto-renders unknown metrics without UI breakage
- Displays recent events and raw NDJSON
- Shows wiring, firmware, and JSON field help for each sensor

## End-to-end data flow

```text
Sensor or input module
  -> SensorModule implementation on Arduino Mega
  -> TelemetryPublisher builds JSON envelope
  -> USB serial stream at 115200 baud
  -> FastAPI transport adapter reads line stream
  -> NDJSON parser + Pydantic validation
  -> Telemetry store + optional JSONL/CSV logging
  -> REST bootstrap + WebSocket push
  -> React telemetry dashboard
```

## Firmware architecture notes

- `mega_telemetry.ino` is intentionally thin.
- `FirmwareApp` owns the transport publisher and the sensor registry.
- Each sensor lives in its own file under `firmware/mega_telemetry/src/sensors/`.
- New sensor modules only need to implement:
  - `begin()`
  - `tick()`
  - `appendSnapshot()`

## Backend architecture notes

- Transport adapters sit under `backend/app/transports/`.
- Runtime orchestration lives in `backend/app/services/runtime.py`.
- Shared catalog data is loaded from `shared/sensor_catalog.json`.
- Bluetooth is already represented as a planned transport in the API, even though USB serial is the only active physical transport in Version 1.

## Frontend architecture notes

- The frontend uses REST for initial state and WebSocket for live updates.
- Dynamic rendering is driven by `recent.latest_metrics` and `recent.numeric_history`.
- Catalog-driven sensor help comes from the backend, which reads the same shared catalog used for documentation.

## Bluetooth next-phase note

Bluetooth is the explicit next step after USB is working.

The intended future data flow is:

```text
Sensor -> Mega -> Bluetooth serial -> FastAPI Bluetooth adapter -> same WebSocket/API -> same React UI
```

That is why the transport layer is separated in both the firmware plan and the backend implementation.

