# Bluetooth Next Phase

USB serial is the first milestone. Bluetooth serial is the immediate next milestone.

The key design rule is simple:

> Preserve the message format and the application pipeline. Change only the transport implementation.

## Current Version 1 flow

```text
Sensor -> Arduino Mega -> Serial (USB) -> FastAPI transport adapter -> parser/store -> WebSocket/API -> React dashboard
```

## Next Bluetooth flow

```text
Sensor -> Arduino Mega -> Serial1 (Bluetooth serial) -> FastAPI Bluetooth adapter -> parser/store -> WebSocket/API -> React dashboard
```

## Recommended firmware migration

1. Introduce a transport abstraction around the current `Stream&` used by `TelemetryPublisher`.
2. Keep `Serial` as the debug channel during the transition.
3. Add a Bluetooth serial module and route the same envelopes to `Serial1`.
4. Reserve Mega pins `18/19` for that work. They are already left free in Version 1.

## Recommended backend migration

1. Add `backend/app/transports/bluetooth_serial.py`.
2. Implement endpoint discovery for paired Bluetooth serial devices.
3. Feed the same NDJSON lines into `TelemetryRuntime._handle_line`.
4. Leave the parsing, logging, catalog, and WebSocket broadcast layers untouched.

## Frontend impact

The frontend should not need a dashboard redesign.

Only expected changes:

- `Bluetooth Serial` becomes a supported transport in `/api/transports`
- Bluetooth endpoints appear in `/api/transports/bluetooth_serial/endpoints`
- The operator selects the Bluetooth endpoint in the same panel already used for USB

## Candidate hardware

- HC-05
- HC-06
- BLE UART bridge modules that present as a serial stream on the host side

The specific Bluetooth hardware can change. The telemetry envelopes should not.

