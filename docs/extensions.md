# Extension Guidance

## Add a new sensor in firmware

1. Create a new module in `firmware/mega_telemetry/src/sensors/`.
2. Inherit from `SensorModule`.
3. Implement:
   - `begin(TelemetryPublisher&)`
   - `tick(unsigned long nowMs, TelemetryPublisher&)`
   - `appendSnapshot(JsonObject metrics, JsonObject sensorStatus)`
4. Add pin constants and polling interval to `src/core/AppConfig.h`.
5. Register the module in `src/core/FirmwareApp.h` and `FirmwareApp.cpp`.
6. Emit stable field names in `metrics`.
7. Emit `event` envelopes for discrete actions if the sensor is event-driven.

## Map new JSON fields through the backend

The backend does not require a hardcoded field list for telemetry metrics.

For most new fields:

1. Keep the envelope shape unchanged.
2. Emit the new metric key from firmware.
3. The backend will validate the envelope and store the metric automatically.

Only extend backend code when you want additional behavior such as:

- derived calculations
- alerting
- custom logging rules
- transport-specific metadata

## Extend the frontend for new fields

The frontend already auto-renders new metrics in the `Unmapped / Newly Discovered Fields` section.

To make a new field feel native:

1. Add the sensor or field to `shared/sensor_catalog.json`.
2. Add the new field to that sensor’s `expected_fields`.
3. Restart the backend so the updated catalog is served.
4. The frontend sensor cards and help panel will pick up the mapping.

## Keep the shared contract in sync

Update these together when you add a sensor:

- `shared/sensor_catalog.json`
- `shared/telemetry_schema.json` if envelope structure changes
- sensor docs under `docs/sensors/`

## Migrate from USB serial to Bluetooth serial

Preserve the telemetry model and only swap the transport implementation.

### Firmware side

- Create a transport wrapper around the current `TelemetryPublisher` output stream.
- Start by duplicating output to `Serial` and `Serial1`.
- Wire the Bluetooth serial module to Mega pins:
  - `TX1` on pin `18`
  - `RX1` on pin `19`
- Keep the same NDJSON envelope format.

### Backend side

- Add a new transport adapter under `backend/app/transports/`.
- Reuse the same `line_handler` and parser path used by USB serial.
- Expose Bluetooth endpoints in `/api/transports` and `/api/transports/{id}/endpoints`.

### Frontend side

- Reuse the existing transport selector.
- Do not change the card rendering or WebSocket event shape.
- Let the operator select `Bluetooth Serial` the same way they select `USB Serial`.

