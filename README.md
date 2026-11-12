# Arduino Sensor Telemetry

Arduino Mega sensor firmware, a Python/FastAPI telemetry backend and a React dashboard with serial and simulated transports.

**Status:** Hardware/software prototype with simulator support. Firmware upload, real wiring, sensor accuracy and serial reliability require physical-board acceptance. Bluetooth is a planned extension.

[Project portfolio](https://zacharysturman.com/projects/arduino-sensor-telemetry)

## Quick start

```sh
cd backend
uv sync
uv run uvicorn app.main:app --host 127.0.0.1 --port 8000
```

## Usage

```sh
cd frontend
pnpm install --frozen-lockfile
pnpm dev
```

## How it works

Firmware emits newline-delimited JSON envelopes. Backend transport adapters normalize serial and simulator data; shared schemas and a sensor catalog connect the backend to the TypeScript dashboard.

## Testing

```sh
cd backend
uv run pytest -q
```

See [validation and next steps](docs/NEXT_STEPS.md) for the checks performed on this snapshot. Automated fixture tests do not establish physical-device compatibility.

## Configuration and privacy

This is a curated source snapshot, published November 12, 2026. Configure your own endpoints, credentials and signing identities. Private development history, local environment files, personal recordings, runtime databases, signing materials and deployment automation are excluded. Deployment and device installation are explicit operator actions.

## Further reading

- [Architecture](docs/architecture.md)
- [Setup](docs/setup.md)
- [Extensions](docs/extensions.md)
- [Bluetooth-Next](docs/bluetooth-next.md)

## License and attribution

Existing licenses and third-party notices apply to the files they cover. Publication of this snapshot does not grant a new license to original material; no additional license is granted where none is supplied.
