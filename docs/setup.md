# Setup And Run Guide

## Prerequisites

- Arduino Mega 2560
- USB cable
- Python `3.12+`
- `uv`
- Node.js `20+`
- `pnpm`
- Arduino IDE

## Backend installation

```bash
cd "/Users/example/Downloads/PORTFOLIO/Technology/Arduino Sensors/backend"
uv venv
uv sync --group dev
```

## Frontend installation

```bash
cd "/Users/example/Downloads/PORTFOLIO/Technology/Arduino Sensors/frontend"
pnpm install
```

## Backend run command

```bash
cd "/Users/example/Downloads/PORTFOLIO/Technology/Arduino Sensors/backend"
uv run uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

## Frontend run command

```bash
cd "/Users/example/Downloads/PORTFOLIO/Technology/Arduino Sensors/frontend"
pnpm dev --host 0.0.0.0 --port 5173
```

## Arduino firmware upload

1. Open Arduino IDE.
2. Install the required libraries:
   - ArduinoJson
   - DHT sensor library
   - NewPing
   - MFRC522
   - IRremote
   - Keypad
   - Encoder
3. Open:

```text
/Users/example/Downloads/PORTFOLIO/Technology/Arduino Sensors/firmware/mega_telemetry/mega_telemetry.ino
```

4. Select `Arduino Mega or Mega 2560`.
5. Select the correct port.
6. Verify and upload.

## Serial port identification

### macOS

```bash
ls /dev/cu.*
```

Plug the Mega in, run the command again, and note the new `/dev/cu.usbmodem*` or `/dev/cu.usbserial*` device.

## End-to-end verification

1. Upload the firmware.
2. Start the backend.
3. Start the frontend.
4. Open the frontend URL printed by Vite.
5. Select `USB Serial`.
6. Select the Mega port.
7. Click `Connect`.
8. Confirm:
   - `Backend online` badge is visible
   - transport status becomes `connected`
   - sensor cards begin filling in
   - raw NDJSON appears
   - button, keypad, RFID, IR, and encoder actions create events

## Simulated mode

1. Start backend and frontend.
2. Select `Simulator`.
3. Choose one of the simulator endpoints.
4. Click `Connect`.
5. Verify live cards, events, and history without needing physical hardware.

## Logging modes

- `off`: no disk logging
- `jsonl`: append validated envelopes to `backend/logs/*.jsonl`
- `jsonl_csv`: write JSONL plus a flattened CSV stream for telemetry snapshots

## Troubleshooting

- If the frontend cannot connect, verify the backend is running on `8000`.
- If USB connect fails, confirm the serial port is not already open in Arduino Serial Monitor.
- If DHT11 readings are blank at startup, wait for warmup and check power.
- If RC522 fails, confirm you used `3.3V`, not `5V`.

