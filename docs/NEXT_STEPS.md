# Validation and next steps

Snapshot prepared November 12, 2026.

## Current state

Hardware/software prototype with simulator support. Firmware upload, real wiring, sensor accuracy and serial reliability require physical-board acceptance. Bluetooth is a planned extension.

## Validation

PASS: 5 backend tests using isolated telemetry inputs. No firmware was flashed and no physical sensors were exercised.

Publication checks: the exact Git tree and its new history are scanned locally with Gitleaks before the repository becomes public. Reviewed scanner exceptions are restricted to synthetic fixture secrets, an invalid test token, a browser preference name and the published NIST AES test vector. Private histories are not included.

## Next action

Follow the README quick start, run the documented focused tests, then use isolated inputs for a manual walkthrough. Real hardware, release signing, background behavior and live-server compatibility are outside this publication check.
